#ifndef RISCV_SIM_DATAMEMORY_H
#define RISCV_SIM_DATAMEMORY_H

#include "Instruction.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <elf.h>
#include <cstring>
#include <vector>
#include <cassert>
#include <map>
#include <unordered_map>

using namespace std;


static constexpr size_t blockCount = 2;
static constexpr size_t memSize = 1024*1024; // memory size in 4-byte words
static constexpr size_t lineSizeBytes = 128;
static constexpr size_t lineSizeWords = lineSizeBytes / sizeof(Word);
static constexpr size_t dataCacheBytes = 4096;
static constexpr size_t codeCacheBytes = 1024;


using Line = std::array<Word, lineSizeWords>;
struct CacheCell {
    Word address;
    Line dataLine;
    Word lastUsage;
};
using Block = vector<CacheCell>;

static Word ToWordAddr(Word ip) { return ip >> 2u; }
static Word ToLineAddr(Word ip) { return ip & ~(lineSizeBytes - 1); }
static Word ToLineOffset(Word ip) { return ToWordAddr(ip) & (lineSizeWords - 1); }

class MemoryStorage {
public:

    MemoryStorage()
    {
        _mem.resize(memSize);
    }

    bool LoadElf(const std::string &elf_filename) {
        std::ifstream elffile;
        elffile.open(elf_filename, std::ios::in | std::ios::binary);

        if (!elffile.is_open()) {
            std::cerr << "ERROR: load_elf: failed opening file \"" << elf_filename << "\"" << std::endl;
            return false;
        }

        elffile.seekg(0, elffile.end);
        size_t buf_sz = elffile.tellg();
        elffile.seekg(0, elffile.beg);

        // Read the entire file. If it doesn't fit in host memory, it won't fit in the risc-v processor
        std::vector<char> buf(buf_sz);
        elffile.read(buf.data(), buf_sz);

        if (!elffile) {
            std::cerr << "ERROR: load_elf: failed reading elf header" << std::endl;
            return false;
        }

        if (buf_sz < sizeof(Elf32_Ehdr)) {
            std::cerr << "ERROR: load_elf: file too small to be a valid elf file" << std::endl;
            return false;
        }

        // make sure the header matches elf32 or elf64
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *) buf.data();
        unsigned char* e_ident = ehdr->e_ident;
        if (e_ident[EI_MAG0] != ELFMAG0
            || e_ident[EI_MAG1] != ELFMAG1
            || e_ident[EI_MAG2] != ELFMAG2
            || e_ident[EI_MAG3] != ELFMAG3) {
            std::cerr << "ERROR: load_elf: file is not an elf file" << std::endl;
            return false;
        }

        if (e_ident[EI_CLASS] == ELFCLASS32) {
            // 32-bit ELF
            return this->LoadElfSpecific<Elf32_Ehdr, Elf32_Phdr>(buf.data(), buf_sz);
        } else if (e_ident[EI_CLASS] == ELFCLASS64) {
            // 64-bit ELF
            return this->LoadElfSpecific<Elf64_Ehdr, Elf64_Phdr>(buf.data(), buf_sz);
        } else {
            std::cerr << "ERROR: load_elf: file is neither 32-bit nor 64-bit" << std::endl;
            return false;
        }
    }

    Word Read(Word ip)
    {
        return _mem[ToWordAddr(ip)];
    }

    void Write(Word ip, Word data)
    {
        _mem[ToWordAddr(ip)] = data;
    }

private:
    template <typename Elf_Ehdr, typename Elf_Phdr>
    bool LoadElfSpecific(char *buf, size_t buf_sz) {
        // 64-bit ELF
        Elf_Ehdr *ehdr = (Elf_Ehdr*) buf;
        Elf_Phdr *phdr = (Elf_Phdr*) (buf + ehdr->e_phoff);
        if (buf_sz < ehdr->e_phoff + ehdr->e_phnum * sizeof(Elf_Phdr)) {
            std::cerr << "ERROR: load_elf: file too small for expected number of program header tables" << std::endl;
            return false;
        }
        auto memptr = reinterpret_cast<char*>(_mem.data());
        // loop through program header tables
        for (int i = 0 ; i < ehdr->e_phnum ; i++) {
            if ((phdr[i].p_type == PT_LOAD) && (phdr[i].p_memsz > 0)) {
                if (phdr[i].p_memsz < phdr[i].p_filesz) {
                    std::cerr << "ERROR: load_elf: file size is larger than memory size" << std::endl;
                    return false;
                }
                if (phdr[i].p_filesz > 0) {
                    if (phdr[i].p_offset + phdr[i].p_filesz > buf_sz) {
                        std::cerr << "ERROR: load_elf: file section overflow" << std::endl;
                        return false;
                    }

                    // start of file section: buf + phdr[i].p_offset
                    // end of file section: buf + phdr[i].p_offset + phdr[i].p_filesz
                    // start of memory: phdr[i].p_paddr
                    std::memcpy(memptr + phdr[i].p_paddr, buf + phdr[i].p_offset, phdr[i].p_filesz);
                }
                if (phdr[i].p_memsz > phdr[i].p_filesz) {
                    // copy 0's to fill up remaining memory
                    size_t zeros_sz = phdr[i].p_memsz - phdr[i].p_filesz;
                    std::memset(memptr + phdr[i].p_paddr + phdr[i].p_filesz, 0, zeros_sz);
                }
            }
        }
        return true;
    }

    std::vector<Word> _mem;
};

class IMem
{
public:
    IMem() = default;
    virtual ~IMem() = default;
    IMem(const IMem &) = delete;
    IMem(IMem &&) = delete;

    IMem& operator=(const IMem&) = delete;
    IMem& operator=(IMem&&) = delete;

    virtual void Request(Word ip) = 0;
    virtual std::optional<Word> Response() = 0;
    virtual void Request(InstructionPtr &instr) = 0;
    virtual bool Response(InstructionPtr &instr) = 0;
    virtual void Clock() = 0;
    virtual size_t getWaitCycles() = 0;
};

class UncachedMem : public IMem
{
public:
    explicit UncachedMem(MemoryStorage& amem)
            : _mem(amem)
    {

    }


    void Request(Word ip) override
    {
        _requestedIp = ip;
        _waitCycles = latency;
    }

    std::optional<Word> Response() override
    {
        if (_waitCycles > 0)
            return std::optional<Word>();
        return _mem.Read(_requestedIp);
    }

    void Request(InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return;

        Request(instr->_addr);
    }

    bool Response(InstructionPtr &instr) override
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return true;

        if (_waitCycles != 0)
            return false;

        if (instr->_type == IType::Ld)
            instr->_data = _mem.Read(instr->_addr);
        else if (instr->_type == IType::St)
            _mem.Write(instr->_addr, instr->_data);

        return true;
    }

    Line readLineFromMemory(Word ip)
    {
        Line memoryLine = { 0 };
        for (int i = 0; i < lineSizeWords; ++i) {
            memoryLine[i] = _mem.Read(ip + 4 * i);
        }

        return memoryLine;
    }

    void writeLineToMemory(Line memoryLine, Word lineAddr)
    {
        for (int i = 0; i < lineSizeWords; ++i) {
            _mem.Write(lineAddr + 4 * i, memoryLine[i]);
        }
    }

    void Clock() override
    {
        if (_waitCycles > 0)
            --_waitCycles;
    }

    size_t getWaitCycles() override
    {
        return _waitCycles;
    }


private:
    static constexpr size_t latency = 120;
    Word _requestedIp = 0;
    size_t _waitCycles = 0;
    MemoryStorage& _mem;
};

// TODO: Create cache for data and for code that works for 1 and 3 ticks
class CachedMem
{
public:
    explicit CachedMem(UncachedMem& uncachedMem): _mem(uncachedMem) {
        initializeCache();
    }

    bool check_key(bool dataOrCode, int key)
    {
        vector<Block> temporalCacheCopy;
        int blockSize = 0;
        if (dataOrCode) {
            temporalCacheCopy = _dataMemory;
            blockSize = dataCacheBytes / lineSizeBytes / blockCount;
        }
        else {
            temporalCacheCopy = _codeMemory;
            blockSize = codeCacheBytes / lineSizeBytes / blockCount;
        }

        for (Block block : temporalCacheCopy)
        {

        }


    }

    Word findEntryWithLowestValue(bool dataOrCode, int size)
    {
        unordered_map<Word, pair<Line, Word>> sampleMap;
        if (dataOrCode)
            sampleMap = _dataMemory;
        else
            sampleMap = _codeMemory;

        if (sampleMap.size() < size)
            return 0;

        Word entryWithMixValue = sampleMap.begin()->second.second;
        Word entryIp = sampleMap.begin()->first;

        unordered_map<Word, pair<Line, Word>>::iterator currentEntry;
        for (currentEntry = sampleMap.begin(); currentEntry != sampleMap.end(); ++currentEntry) {
            if (currentEntry->second.second < entryWithMixValue) {
                entryWithMixValue = currentEntry->second.second;
                entryIp = currentEntry->first;
            }
        }
        return entryIp;
    }

    void Request(Word ip)
    {
        if (ip != _memoryRequestIp) {
            _memoryRequestIp = ip;
            Word lineAddr = ToLineAddr(_memoryRequestIp);
            Word offset = ToLineOffset(_memoryRequestIp);

            if (check_key(_codeMemory, lineAddr)) {
                _waitCycles = codeLatency;
                _cacheMiss = false;
            }
            else {
                _cacheMiss = true;
                _waitCycles = failLatency;
            }
            _requestedIp = lineAddr;
            _requestedOffset = offset;
        }
    }

    std::optional<Word> Response(Word responseTime)
    {
        if (_waitCycles > 0)
            return std::optional<Word>();

        if (_cacheMiss)
        {
            Line memoryLine = _mem.readLineFromMemory(_requestedIp);

            Word latestUsage = findEntryWithLowestValue(false, codeCacheBytes / lineSizeWords);

//            Word writeAddress = latestUsage;
//            if (latestUsage == 0) {
//                writeAddress = _requestedIp;
//                _codeMemory[writeAddress] = std::pair<Line, Word>(memoryLine, responseTime);
//            }
            if (latestUsage != 0)
            {
                _mem.writeLineToMemory(_codeMemory[latestUsage].first, latestUsage);
                _codeMemory.erase(latestUsage);
            }

            _codeMemory[_requestedIp] = std::pair<Line, Word>(memoryLine, responseTime);
            return memoryLine[_requestedOffset];

        } else {
//            _lastCodeUsage[_requestedIp] = responseTime;
            _codeMemory[_requestedIp].second = responseTime;
            return _codeMemory[_requestedIp].first[_requestedOffset];
        }
    }

    void Request(InstructionPtr &instr)
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return;

        Word lineAddr = ToLineAddr(instr->_addr);
        Word offset = ToLineOffset(instr->_addr);

        if (check_key(_dataMemory, lineAddr)) {
            _waitCycles = dataLatency;
            _cacheMiss = false;
        } else {
            _cacheMiss = true;
            _waitCycles = failLatency;
            if (instr->_type == IType::St && (findEntryWithLowestValue(true, dataCacheBytes / lineSizeWords)))
                _waitCycles += 120;
        }
        _requestedIp = lineAddr;
        _requestedOffset = offset;
    }

    bool Response(InstructionPtr &instr, Word responseTime)
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return true;

        if (_waitCycles != 0)
            return false;

        if (_cacheMiss)
        {
            Line memoryLine = _mem.readLineFromMemory(_requestedIp);

            if (instr->_type == IType::St)
                memoryLine[ToLineOffset(instr->_addr)] = instr->_data;

            Word latestUsage = findEntryWithLowestValue(true, dataCacheBytes / lineSizeWords);

            if (latestUsage != 0)
            {
                _mem.writeLineToMemory(_dataMemory[latestUsage].first, latestUsage);
                _codeMemory.erase(latestUsage);
            }

            _dataMemory[_requestedIp] = std::pair<Line, Word>(memoryLine, responseTime);

            if (instr->_type == IType::Ld)
                instr->_data = memoryLine[_requestedOffset];
        }
        else
        {
            _dataMemory[_requestedIp].second = responseTime;

            if (instr->_type == IType::Ld)
                instr->_data = _dataMemory[_requestedIp].first[_requestedOffset];
            else if (instr->_type == IType::St)
                _dataMemory[_requestedIp].first[_requestedOffset] = instr->_data;
        }

        return true;
    }

    void initializeCache()
    {
        for (int i = 0; i < blockCount; ++i) {
            _codeMemory1[i] = vector<CacheCell>(codeCacheBytes / lineSizeBytes / blockCount);
            _dataMemory1[i] = vector<CacheCell>(dataCacheBytes / lineSizeBytes / blockCount);
        }
    }

    void Clock()
    {
        if (_waitCycles > 0)
            --_waitCycles;
    }

    size_t getWaitCycles()
    {
        return _waitCycles;
    }
private:
    static constexpr size_t failLatency = 152;
    static constexpr size_t codeLatency = 1;
    static constexpr size_t dataLatency = 3;

    Word _memoryRequestIp = 0;
    Word _requestedIp = 0;
    Word _requestedOffset = 0;
    size_t _waitCycles = 0;
    bool _cacheMiss = false;


    //TODO: Structure <Line address, <Line, Last usage>>
//    unordered_map<Word, pair<Line, Word>> _dataMemory = unordered_map<Word, pair<Line, Word>>();
//    unordered_map<Word, pair<Line, Word>> _codeMemory = unordered_map<Word, pair<Line, Word>>();

    vector<Block> _codeMemory = vector<Block>(blockCount);
    vector<Block> _dataMemory = vector<Block>(blockCount);
    UncachedMem& _mem;
};

#endif //RISCV_SIM_DATAMEMORY_H