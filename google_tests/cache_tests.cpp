//
// Created by shevchenya on 14/11/2020.
//

#include <memory>

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

TEST_F(CacheMemoryFixture, IsCacheCreated) {
    CachedMem cache = *memModelPtr;
    ASSERT_GT(cache.getDataMemory().size(), 0);
    ASSERT_GT(cache.getCodeMemory().size(), 0);
    ASSERT_EQ(cache.getWaitCycles(), 0);

    for (const auto & i : cache.getCodeMemory()[0])
        ASSERT_EQ(i.address, 0);


    for (int i = 0; i < cache.getDataMemory()[0].size(); i++)
        ASSERT_EQ(cache.getDataMemory()[2][i].address, 0);
}

TEST_F(CacheMemoryFixture, ChalangeCache) {
    CachedMem cache = *memModelPtr;

}
