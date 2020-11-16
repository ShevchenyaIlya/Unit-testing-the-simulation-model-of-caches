//
// Created by shevchenya on 15/11/2020.
//

#include <random>

#include "gtest/gtest.h"
#include "../src/Memory.h"
#include <optional>

class CacheMemoryFixture : public ::testing::Test {
protected:
    void SetUp() override {
        MemoryStorage mem;
        UncachedMem uncachedMem = UncachedMem (mem);
        memModelPtr = std::make_unique<CachedMem> ( uncachedMem);
    }
    std::unique_ptr<CachedMem> memModelPtr;
};

TEST_F(CacheMemoryFixture, waitResponse) {
    CachedMem cache = *memModelPtr;
    cache.setWaitCycles(5);

    ASSERT_EQ(cache.Response(20), std::optional<Word>());
}

TEST_F(CacheMemoryFixture, cacheMiss) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].dataLine[requestOffset] = 1024;
    cache.setCacheMiss(false);

    ASSERT_EQ(cache.Response(20), 1024);
    ASSERT_EQ(cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
}

TEST_F(CacheMemoryFixture, fullCacheMiss) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 520;
    Word requestOffset = ToLineOffset(requestedId);
    cache.setRequestedIp(ToLineAddr(requestedId));
    cache.setRequestedOffset(requestOffset);
    cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].dataLine[requestOffset] = 1024;
    cache.setCacheMiss(false);

    for (Block &block : cache.getCodeMemory()) {
        block[requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].lastUsage = 1;
    }


    ASSERT_EQ(cache.Response(20), 1024);
    ASSERT_EQ(cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);

    for (int i = 0; i < cache.getCodeMemory().size(); i++) {
        if (i == blockLine)
            ASSERT_EQ(cache.getCodeMemory()[i][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
        else
            ASSERT_EQ(cache.getCodeMemory()[i][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].lastUsage, 0);
    }
}

TEST_F(CacheMemoryFixture, EmptyCacheHit) {
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
    cache.setCacheMiss(true);

    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[0][0].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][0].address, ToLineAddr(requestedId));
    ASSERT_TRUE(cache.getCodeMemory()[0][0].validityBit);
}

TEST_F(CacheMemoryFixture, FullCacheHit) {
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
    cache.setCacheMiss(true);
    for (Block &block : cache.getCodeMemory()) {
        Word address = requestedId % (codeCacheBytes / blockCount) / lineSizeBytes;
        block[address].address = 1;
        Line emptyLine = Line();
        emptyLine[0] = 1024;
        block[address].dataLine = emptyLine;
    }

    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[0][0].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][0].address, ToLineAddr(requestedId));
    ASSERT_TRUE(cache.getCodeMemory()[0][0].validityBit);

    ASSERT_EQ(cache.getMem().readLineFromMemory(0)[0], 1024);
}