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

TEST_F(CacheMemoryFixture, FindEntryTests) {
    CachedMem cache = *memModelPtr;
    pair<Word, Word> emptyValue(0, 0);

    int requestedId = rand() % 1024;
    ASSERT_EQ(cache.findEntry(requestedId, true), emptyValue);
    ASSERT_EQ(cache.findEntry(requestedId, false), emptyValue);

    int requestedBlock = rand() % blockCount;
    pair<Word, Word> destinationValue(requestedBlock, (requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes);
    cache.getDataMemory()[requestedBlock][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].address = requestedId;

    ASSERT_EQ(cache.findEntry(requestedId, true), destinationValue);

    destinationValue = pair<Word, Word>(requestedBlock, (requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes);
    cache.getCodeMemory()[requestedBlock][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].address = requestedId;

    ASSERT_EQ(cache.findEntry(requestedId, false), destinationValue);
}

TEST_F(CacheMemoryFixture, PseudoLRUFindingTestsEmptyCache) {
    CachedMem cache = *memModelPtr;

    Word requestedId = rand() % 1024;
    pair<Word, Word> destinationValue(0, (requestedId % (dataCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(true, requestedId), destinationValue);

    destinationValue = pair<Word, Word>(0, (requestedId % (codeCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(false, requestedId), destinationValue);

}

TEST_F(CacheMemoryFixture, PseudoLRUFindingTestsOneValueCache) {
    CachedMem cache = *memModelPtr;

    Word requestedId = rand() % 1024;
    cache.getDataMemory()[0][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;
    cache.getCodeMemory()[0][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;

    pair<Word, Word>destinationValue = pair<Word, Word>(1, (requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(true, requestedId), destinationValue);
    destinationValue = pair<Word, Word>(1, (requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(false, requestedId), destinationValue);
}

TEST_F(CacheMemoryFixture, PseudoLRUFindingTestsFullCache) {
    CachedMem cache = *memModelPtr;

    Word requestedId = rand() % 1024;
    Word destinationId = rand() % blockCount;
    for (int i = 0; i < cache.getDataMemory().size(); i++) {
        cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
        if (i != destinationId)
            cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;
    }

    pair<Word, Word>destinationValue = pair<Word, Word>(destinationId, (requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(true, requestedId), destinationValue);

    for (int i = 0; i < cache.getCodeMemory().size(); i++) {
        cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
        if (i != destinationId)
            cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;
    }

    destinationValue = pair<Word, Word>(destinationId, (requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes);
    ASSERT_EQ(cache.pseudoLRUFinding(false, requestedId), destinationValue);
}

TEST_F(CacheMemoryFixture, PseudoLRUFindingTestsCacheUpdate) {
    CachedMem cache = *memModelPtr;

    Word requestedId = rand() % 1024;
    Word destinationId = rand() % blockCount;
    for (int i = 0; i < cache.getDataMemory().size(); i++) {
        cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
        if (i != destinationId)
            cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;
    }

    cache.pseudoLRUFinding(true, requestedId);
    for (int i = 0; i < cache.getDataMemory().size(); i++) {
        ASSERT_EQ(cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].lastUsage, 0);
    }

    for (int i = 0; i < cache.getCodeMemory().size(); i++) {
        cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
        if (i != destinationId)
            cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].lastUsage = 1;
    }

    cache.pseudoLRUFinding(false, requestedId);
    for (int i = 0; i < cache.getCodeMemory().size(); i++) {
        ASSERT_EQ(cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].lastUsage, 0);
    }
}

TEST_F(CacheMemoryFixture, PseudoLRUFindingTestsChooseChangedLine) {
    CachedMem cache = *memModelPtr;

    Word requestedId = rand() % 1024;
    Word destinationId = rand() % blockCount;

    for (int i = 0; i < cache.getDataMemory().size(); i++) {
        cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].address = i + 1;
        cache.getDataMemory()[i][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
    }

    pair<Word, Word>destinationValue = pair<Word, Word>(destinationId, (requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes);
    cache.getDataMemory()[destinationId][(requestedId % ( dataCacheBytes / blockCount)) / lineSizeBytes].validityBit = false;
    ASSERT_EQ(cache.pseudoLRUFinding(true, requestedId), destinationValue);

    for (int i = 0; i < cache.getCodeMemory().size(); i++) {
        cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].address = i + 1;
        cache.getCodeMemory()[i][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].validityBit = true;
    }

    destinationValue = pair<Word, Word>(destinationId, (requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes);
    cache.getCodeMemory()[destinationId][(requestedId % ( codeCacheBytes / blockCount)) / lineSizeBytes].validityBit = false;
    ASSERT_EQ(cache.pseudoLRUFinding(false, requestedId), destinationValue);
}