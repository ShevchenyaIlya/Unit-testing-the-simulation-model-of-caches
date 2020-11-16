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


static constexpr size_t blockCount = 4;
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
    bool validityBit;
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

    bool checkAddress(bool dataOrCode, int address)
    {
        vector<Block> temporalCacheCopy;
        int blockSize = 0;
        if (dataOrCode) {
            temporalCacheCopy = _dataMemory;
            blockSize = dataCacheBytes / blockCount;
        }
        else {
            temporalCacheCopy = _codeMemory;
            blockSize = codeCacheBytes / blockCount;
        }

        for (Block block : temporalCacheCopy)
        {
            if (block[(address % blockSize) / lineSizeBytes].address == address)
                return true;
        }
        return false;
    }


    static bool checkAllBits(vector<Block> &cache, Word key, unsigned long indexing)
    {
        for (int i = 0; i < blockCount; i++)
        {
            if (i != key && cache[i][indexing].lastUsage == 0)
            {
                return false;
            }
        }
        return true;
    }

    static void changeLRUBit(vector<Block> &cache, Word key, unsigned long indexing)
    {
        if (checkAllBits(cache, key, indexing))
        {
            for (int j = 0; j < blockCount; j++)
            {
                if (key != j)
                    cache[j][indexing].lastUsage = 0;
            }
        }
    }

    static bool ifAllMarked(vector<Block> &cache, unsigned long indexing)
    {
        for (int i = 0; i < blockCount; i++)
        {
            if (cache[i][indexing].address == 0)
            {
                return false;
            }
        }
        return true;
    }
    
    pair<Word, Word> pseudoLRUFinding(bool dataOrCode, int key)
    {
        vector<Block> *temporalCacheCopy;
        int blockSize = 0;
        if (dataOrCode) {
            temporalCacheCopy = &_dataMemory;
            blockSize = dataCacheBytes / blockCount;
        }
        else {
            temporalCacheCopy = &_codeMemory;
            blockSize = codeCacheBytes / blockCount;
        }
        

        for (int i = 0; i < blockCount; i++) {
            if (!(*temporalCacheCopy)[i][(key % blockSize) / lineSizeBytes].validityBit && ifAllMarked(*temporalCacheCopy, (key % blockSize) / lineSizeBytes)) {
                return pair<Word, Word>(i, (key % blockSize) / lineSizeBytes);
            }
        }

        Word minEntryValue = (*temporalCacheCopy)[0][(key % blockSize) / lineSizeBytes].lastUsage;
        Word entryIdentification = 0;
        for (int i = 0; i < blockCount; i++)
        {
            if ((*temporalCacheCopy)[i][(key % blockSize) / lineSizeBytes].lastUsage < minEntryValue)
            {
                minEntryValue = (*temporalCacheCopy)[i][(key % blockSize) / lineSizeBytes].lastUsage;
                entryIdentification = i;
            }
        }

        changeLRUBit(*temporalCacheCopy, entryIdentification, (key % blockSize) / lineSizeBytes);

        return pair<Word, Word>(entryIdentification, (key % blockSize) / lineSizeBytes);
    }


    pair<Word, Word> findEntry(int requestedId, bool dataOrCode) {
        vector<Block> temporalCacheCopy;
        int blockSize = 0;
        if (dataOrCode) {
            temporalCacheCopy = _dataMemory;
            blockSize = dataCacheBytes / blockCount;
        }
        else {
            temporalCacheCopy = _codeMemory;
            blockSize = codeCacheBytes / blockCount;
        }

        for (int i = 0; i < blockCount; i++)
        {
            if (temporalCacheCopy[i][(requestedId % blockSize) / lineSizeBytes].address == requestedId)
                return pair<Word, Word> (i, (requestedId % blockSize) / lineSizeBytes);
        }
        return pair<Word, Word> (0, 0);
    }

    void Request(Word ip)
    {
        if (ip != _memoryRequestIp) {
            _memoryRequestIp = ip;
            Word lineAddr = ToLineAddr(_memoryRequestIp);
            Word offset = ToLineOffset(_memoryRequestIp);

            if (checkAddress(false, lineAddr)) {
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

            pair<Word, Word> latestUsage = pseudoLRUFinding(false, _requestedIp);
            Word blockId = latestUsage.first;
            Word cellId = latestUsage.second;

            if (_codeMemory[blockId][cellId].address != 0)
            {
                _mem.writeLineToMemory(_codeMemory[blockId][cellId].dataLine,
                                       _codeMemory[blockId][cellId].address);
            }

            _codeMemory[blockId][cellId] = CacheCell {_requestedIp, memoryLine, 1, true};

            return memoryLine[_requestedOffset];

        } else {
            pair<Word, Word> position = findEntry(_requestedIp, false);
            _codeMemory[position.first][position.second].lastUsage = 1;
            changeLRUBit(_codeMemory, position.first, position.second);

            return _codeMemory[position.first][position.second].dataLine[_requestedOffset];
        }
    }

    void Request(InstructionPtr &instr)
    {
        if (instr->_type != IType::Ld && instr->_type != IType::St)
            return;

        Word lineAddr = ToLineAddr(instr->_addr);
        Word offset = ToLineOffset(instr->_addr);

        if (checkAddress(true, lineAddr)) {
            _waitCycles = dataLatency;
            _cacheMiss = false;
        } else {
            _cacheMiss = true;
            _waitCycles = failLatency;
            pair<Word, Word> latestUsage = pseudoLRUFinding(true, _requestedIp);
            if (instr->_type == IType::St && ((_dataMemory[latestUsage.first][latestUsage.second].address != 0) && !_dataMemory[latestUsage.first][latestUsage.second].validityBit))
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

            bool validity = true;
            if (instr->_type == IType::St) {
                memoryLine[ToLineOffset(instr->_addr)] = instr->_data;
                validity = false;
            }

            pair<Word, Word> latestUsage = pseudoLRUFinding(true, _requestedIp);
            Word blockId = latestUsage.first;
            Word cellId = latestUsage.second;

            if (_dataMemory[blockId][cellId].address != 0 && !_dataMemory[blockId][cellId].validityBit)
            {
                _mem.writeLineToMemory(_dataMemory[blockId][cellId].dataLine, _dataMemory[blockId][cellId].address);
            }

            _dataMemory[blockId][cellId] = CacheCell {_requestedIp, memoryLine, 1, validity};

            if (instr->_type == IType::Ld)
                instr->_data = memoryLine[_requestedOffset];
        }
        else
        {
            pair<Word, Word> position = findEntry(_requestedIp, true);
            _dataMemory[position.first][position.second].lastUsage = 1;
            changeLRUBit(_dataMemory, position.first, position.second);

            if (instr->_type == IType::Ld)
                instr->_data = _dataMemory[position.first][position.second].dataLine[_requestedOffset];
            else if (instr->_type == IType::St) {
                _dataMemory[position.first][position.second].dataLine[_requestedOffset] = instr->_data;
                _dataMemory[position.first][position.second].validityBit = false;
            }
        }

        return true;
    }

    void initializeCache()
    {
        for (int i = 0; i < blockCount; ++i) {
            _codeMemory[i] = vector<CacheCell>(codeCacheBytes / lineSizeBytes / blockCount);
            _dataMemory[i] = vector<CacheCell>(dataCacheBytes / lineSizeBytes / blockCount);
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

    static const size_t getFailLatency() {
        return failLatency;
    }

    static const size_t getCodeLatency() {
        return codeLatency;
    }

    static const size_t getDataLatency() {
        return dataLatency;
    }

    void setWaitCycles(size_t waitCycles) {
        _waitCycles = waitCycles;
    }

    bool isCacheMiss() const {
        return _cacheMiss;
    }

    vector<Block> &getCodeMemory() {
        return _codeMemory;
    }

    vector<Block> &getDataMemory(){
        return _dataMemory;
    }

    Word getMemoryRequestIp() const {
        return _memoryRequestIp;
    }

    Word getRequestedIp() const {
        return _requestedIp;
    }

    Word getRequestedOffset() const {
        return _requestedOffset;
    }

    void setMemoryRequestIp(Word memoryRequestIp) {
        _memoryRequestIp = memoryRequestIp;
    }

    void setCacheMiss(bool cacheMiss) {
        _cacheMiss = cacheMiss;
    }

    UncachedMem &getMem() const {
        return _mem;
    }

    void setRequestedIp(Word requestedIp) {
        _requestedIp = requestedIp;
    }

    void setRequestedOffset(Word requestedOffset) {
        _requestedOffset = requestedOffset;
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

    vector<Block> _codeMemory = vector<Block>(blockCount);
    vector<Block> _dataMemory = vector<Block>(blockCount);
    UncachedMem& _mem;
};

#endif //RISCV_SIM_DATAMEMORY_H