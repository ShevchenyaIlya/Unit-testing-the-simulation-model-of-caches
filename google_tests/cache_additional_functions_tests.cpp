//
// Created by shevchenya on 15/11/2020.
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

TEST_F(CacheMemoryFixture, CheckAddressTests) {
    CachedMem cache = *memModelPtr;
    ASSERT_FALSE(cache.checkAddress(true, 512));
    ASSERT_FALSE(cache.checkAddress(false, 1024));

    cache.getDataMemory()[0][4].address = 512;
    cache.getCodeMemory()[0][0].address = 1024;

    ASSERT_TRUE(cache.checkAddress(true, 512));
    ASSERT_TRUE(cache.checkAddress(false, 1024));

    cache.getDataMemory()[0][4].address = 511;
    cache.getDataMemory()[1][4].address = 512;
    cache.getCodeMemory()[0][0].address = 128;
    cache.getCodeMemory()[1][0].address = 1024;

    ASSERT_TRUE(cache.checkAddress(true, 512));
    ASSERT_TRUE(cache.checkAddress(false, 1024));

    for(Block block : cache.getCodeMemory())
    {
        for(int i = 0; i < block.size(); i++)
        {
            Word randomValue = rand() % 1024;
            if (randomValue == 512)
                randomValue += 12;
            block[i].address = randomValue;
        }
    }

    ASSERT_FALSE(cache.checkAddress(false, 512));

    for(Block block : cache.getDataMemory())
    {
        for(int i = 0; i < block.size(); i++)
        {
            Word randomValue = rand() % 2048;
            if (randomValue == 1024)
                randomValue++;
            block[i].address = randomValue;
        }
    }

    ASSERT_FALSE(cache.checkAddress(true, 1024));
}


TEST_F(CacheMemoryFixture, CheckAllBitsTests) {
    CachedMem cache = *memModelPtr;
    ASSERT_TRUE(cache.checkAllBits(cache.getCodeMemory(), 512, 12));
    ASSERT_TRUE(cache.checkAllBits(cache.getDataMemory(), 512, 12));
}


TEST_F(CacheMemoryFixture, ChangeLRUBitTests) {
    CachedMem cache = *memModelPtr;
    ASSERT_TRUE(cache.checkAllBits(cache.getCodeMemory(), 512, 12));
    ASSERT_TRUE(cache.checkAllBits(cache.getDataMemory(), 512, 12));
}


TEST_F(CacheMemoryFixture, IfAllMarkedTests) {
    CachedMem cache = *memModelPtr;
    ASSERT_FALSE(cache.ifAllMarked(cache.getDataMemory(), (512 % (dataCacheBytes / blockCount) / lineSizeBytes)));
    ASSERT_FALSE(cache.ifAllMarked(cache.getCodeMemory(), (512 % (codeCacheBytes / blockCount) / lineSizeBytes)));

    for (Block &block : cache.getDataMemory()) {
        block[(512 % (dataCacheBytes / blockCount)) / lineSizeBytes].address = rand() % 1024 + 1;
    }

    ASSERT_TRUE(cache.ifAllMarked(cache.getDataMemory(), (512 % (dataCacheBytes / blockCount) / lineSizeBytes)));

    for (Block &block : cache.getCodeMemory()) {
        block[(512 % (codeCacheBytes / blockCount) / lineSizeBytes)].address = rand() % 1024 + 1;
    }

    ASSERT_TRUE(cache.ifAllMarked(cache.getCodeMemory(), (512 % (codeCacheBytes / blockCount) / lineSizeBytes)));

    cache.getCodeMemory()[rand() % blockCount][(512 % (codeCacheBytes / blockCount) / lineSizeBytes)].address = 0;
    cache.getDataMemory()[rand() % blockCount][(512 % (dataCacheBytes / blockCount) / lineSizeBytes)].address = 0;

    ASSERT_FALSE(cache.ifAllMarked(cache.getDataMemory(), (512 % (dataCacheBytes / blockCount) / lineSizeBytes)));
    ASSERT_FALSE(cache.ifAllMarked(cache.getCodeMemory(), (512 % (codeCacheBytes / blockCount) / lineSizeBytes)));
}