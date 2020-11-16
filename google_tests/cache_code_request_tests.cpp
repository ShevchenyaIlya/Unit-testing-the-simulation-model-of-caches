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

TEST_F(CacheMemoryFixture, waitRequest) {
    CachedMem cache = *memModelPtr;

    cache.setMemoryRequestIp(512);
    cache.Request(512);
    ASSERT_EQ(cache.getRequestedIp(), 0);
    ASSERT_EQ(cache.getRequestedOffset(), 0);
}

TEST_F(CacheMemoryFixture, lineInCache) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = rand() % 1024;
    cache.getCodeMemory()[blockLine][requestedId % (codeCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.Request(requestedId);
    ASSERT_FALSE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getCodeLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(requestedId));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(requestedId));
}

TEST_F(CacheMemoryFixture, lineNotInCache) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = rand() % 1024;
    cache.Request(requestedId);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(requestedId));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(requestedId));
}