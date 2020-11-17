//
// Created by shevchenya on 15/11/2020.
//

#include <random>

#include "gtest/gtest.h"
#include "../src/Memory.h"

class CacheMemoryFixture : public ::testing::Test {
protected:
    void SetUp() override {
        MemoryStorage mem ;
        UncachedMem uncachedMem = UncachedMem (mem);
        memModelPtr = std::make_unique<CachedMem> ( uncachedMem);
    }
    std::unique_ptr<CachedMem> memModelPtr;
};

TEST_F(CacheMemoryFixture, waitDataResponse) {
    CachedMem cache = *memModelPtr;
    cache.setWaitCycles(5);

    Word data = 200;
    ASSERT_FALSE(cache.Response(512, data, IType::St, 24));
}

TEST_F(CacheMemoryFixture, incorrectCommandType) {
    CachedMem cache = *memModelPtr;
    cache.setWaitCycles(5);

    Word data = 200;
    ASSERT_TRUE(cache.Response(512, data, IType::J, 24));
    ASSERT_TRUE(cache.Response(512, data, IType::Unsupported, 24));
}

TEST_F(CacheMemoryFixture, cacheHitStoreCommand) {
    MemoryStorage mem;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[requestOffset] = 1024;
    cache.setCacheMiss(false);

    Word data = 1024;
    ASSERT_TRUE(cache.Response(requestedId, data, IType::St, 5));
    ASSERT_FALSE(cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[requestOffset], 1024);
}

TEST_F(CacheMemoryFixture, cacheHitLoadCommand) {
    MemoryStorage mem;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[requestOffset] = 1024;
    cache.setCacheMiss(false);

    Word data = 0;
    ASSERT_TRUE(cache.Response(requestedId, data, IType::Ld, 5));
    ASSERT_EQ(cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(data, 1024);
}

TEST_F(CacheMemoryFixture, emptyCacheHitStoreCommand) {
    MemoryStorage mem;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
    CachedMem cache = *memModelPtr;

    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.setCacheMiss(true);

    Word data = 0;
    Word blockCell = requestedId % (dataCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_TRUE(cache.Response(requestedId, data, IType::St, 5));
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].address, ToLineAddr(requestedId));
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].dataLine[requestOffset], data);
    ASSERT_FALSE(cache.getDataMemory()[0][blockCell].validityBit);

    ASSERT_TRUE(cache.Response(requestedId, data, IType::Ld, 5));
    ASSERT_EQ(cache.getDataMemory()[1][blockCell].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[1][blockCell].address, ToLineAddr(requestedId));
    ASSERT_TRUE(cache.getDataMemory()[1][blockCell].validityBit);
    ASSERT_EQ(data, 19);
}

TEST_F(CacheMemoryFixture, fullDataCacheHitStoreCommand) {
    MemoryStorage mem;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
    CachedMem cache = *memModelPtr;

    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.setCacheMiss(true);
    for (Block &block : cache.getDataMemory()) {
        Word address = requestedId % (dataCacheBytes / blockCount) / lineSizeBytes;
        block[address].address = 1;
        Line emptyLine = Line();
        emptyLine[0] = 1024;
        block[address].dataLine = emptyLine;
    }

    Word data = 0;
    Word blockCell = requestedId % (dataCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_TRUE(cache.Response(requestedId, data, IType::St, 5));
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].address, ToLineAddr(requestedId));
    ASSERT_EQ(cache.getDataMemory()[0][blockCell].dataLine[requestOffset], data);
    ASSERT_FALSE(cache.getDataMemory()[0][blockCell].validityBit);

    ASSERT_EQ(cache.getMem().readLineFromMemory(0)[0], 1024);
}