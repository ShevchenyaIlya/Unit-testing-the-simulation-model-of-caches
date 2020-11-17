//
// Created by shevchenya on 15/11/2020.
//

#include <random>

#include "gtest/gtest.h"
#include "../src/Memory.h"
#include "../src/Instruction.h"
#include "../src/Decoder.h"

class CacheMemoryFixture : public ::testing::Test {
protected:
    void SetUp() override {
        MemoryStorage mem ;
        UncachedMem uncachedMem = UncachedMem (mem);
        memModelPtr = std::make_unique<CachedMem> ( uncachedMem);
    }
    std::unique_ptr<CachedMem> memModelPtr;
};

TEST_F(CacheMemoryFixture, notLoadOrStoreCommands) {
    CachedMem cache = *memModelPtr;

    cache.setMemoryRequestIp(512);
    cache.Request(512, IType::J);
    ASSERT_EQ(cache.getRequestedIp(), 0);
    ASSERT_EQ(cache.getRequestedOffset(), 0);
    ASSERT_EQ(cache.getWaitCycles(), 0);

    cache.Request(512, IType::Unsupported);
    ASSERT_EQ(cache.getRequestedIp(), 0);
    ASSERT_EQ(cache.getRequestedOffset(), 0);
    ASSERT_EQ(cache.getWaitCycles(), 0);
}

TEST_F(CacheMemoryFixture, cacheHit) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 516;
    cache.getDataMemory()[blockLine][requestedId % (dataCacheBytes / blockCount) / lineSizeBytes].address = ToLineAddr(requestedId);
    cache.Request(requestedId, IType::St);
    ASSERT_FALSE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getDataLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(requestedId));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(requestedId));
}

TEST_F(CacheMemoryFixture, requestMissStoreCommand) {
    CachedMem cache = *memModelPtr;

    int blockLine = rand() % blockCount;
    Word requestedId = 520;
    cache.Request(requestedId, IType::Ld);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(requestedId));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(requestedId));

    cache.Request(requestedId, IType::St);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(requestedId));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(requestedId));
}
