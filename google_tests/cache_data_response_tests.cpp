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