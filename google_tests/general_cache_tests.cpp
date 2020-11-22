//
// Created by shevchenya on 17/11/2020.
//

#include "gtest/gtest.h"
#include "../src/Memory.h"

TEST(GeneralCacheTests, emptyCodeDataRequestResponse) {
    MemoryStorage mem;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
    CachedMem cache = *memModelPtr;

    //Code cache first request
    cache.Request(512);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(512));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(512));

    //Code cache response
    cache.setWaitCycles(0);
    Word position = 512 % (codeCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[0][position].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][position].address, ToLineAddr(512));
    ASSERT_TRUE(cache.getCodeMemory()[0][position].validityBit);

    //Data cache first request
    cache.Request(1024, IType::St);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(1024));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(1024));
    cache.setWaitCycles(0);

    //Data cache response
    Word responseData = 1024;
    ASSERT_TRUE(cache.Response(1024, responseData, IType::St, 20));
    ASSERT_FALSE(cache.getDataMemory()[0][1024 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[0][1024 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][1024 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(1024)], 1024);

    //Line in cache. Second code request/response
    cache.Request(516);
    ASSERT_FALSE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getCodeLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(516));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(516));

    cache.setWaitCycles(0);
    position = 516 % (codeCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[0][position].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][position].address, ToLineAddr(516));
    ASSERT_TRUE(cache.getCodeMemory()[0][position].validityBit);

    //Line in cache. Second data request/response
    cache.Request(1028, IType::St);
    ASSERT_FALSE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getDataLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(1028));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(1028));

    cache.setWaitCycles(0);
    responseData = 1028;
    ASSERT_TRUE(cache.Response(1028, responseData, IType::St, 20));
    ASSERT_FALSE(cache.getDataMemory()[0][1028 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[0][1028 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][1028 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(1028)], 1028);

    //New line not in cache. Third code request/response
    cache.Request(640);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(640));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(640));

    cache.setWaitCycles(0);
    position = 640 % (codeCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[0][position].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][position].address, ToLineAddr(640));
    ASSERT_TRUE(cache.getCodeMemory()[0][position].validityBit);

    //New line not in cache. Third data request/response
    cache.Request(1152, IType::St);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(1152));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(1152));

    cache.setWaitCycles(0);
    responseData = 1152;
    ASSERT_TRUE(cache.Response(1152, responseData, IType::St, 20));
    ASSERT_FALSE(cache.getDataMemory()[0][1152 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[0][1152 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][1152 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(1152)], 1152);

    //Second code block hit.
    cache.Request(768);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(768));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(768));

    cache.setWaitCycles(0);
    position = 768 % (codeCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_EQ(cache.Response(20), 19);
    ASSERT_EQ(cache.getCodeMemory()[1][position].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[1][position].address, ToLineAddr(768));
    ASSERT_TRUE(cache.getCodeMemory()[1][position].validityBit);

    //Second data block hit.
    cache.Request(2048, IType::St);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(2048));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(2048));

    cache.setWaitCycles(0);
    responseData = 2048;
    ASSERT_TRUE(cache.Response(2048, responseData, IType::St, 20));
    ASSERT_FALSE(cache.getDataMemory()[1][2048 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[1][2048 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[1][2048 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(2048)], 2048);

    //Fill code cache
    for (int i = 0; i < blockCount; i++) {
        cache.Request(512 + i * 128 * 2);
        cache.setWaitCycles(0);
        cache.Response(20);
    }

    ASSERT_EQ(cache.getCodeMemory()[blockCount - 1][0].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[blockCount - 1][0].address, ToLineAddr(1280));
    ASSERT_TRUE(cache.getCodeMemory()[blockCount - 1][0].validityBit);

    //Add line to full cache
    cache.Request(1536);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency());
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(1536));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(1536));

    cache.setWaitCycles(0);
    position = 1536 % (codeCacheBytes / blockCount) / lineSizeBytes;
    ASSERT_EQ(cache.Response(20), 0);
    ASSERT_EQ(cache.getCodeMemory()[0][position].lastUsage, 1);
    ASSERT_EQ(cache.getCodeMemory()[0][position].address, ToLineAddr(1536));
    ASSERT_TRUE(cache.getCodeMemory()[0][position].validityBit);


    //Fill code cache
    for (int i = 0; i < blockCount; i++) {
        cache.Request(1024 + i * dataCacheBytes / blockCount, IType::St);
        cache.setWaitCycles(0);
        cache.Response(1024 + i * dataCacheBytes / blockCount, responseData, IType::St, 20);
    }

    ASSERT_FALSE(cache.getDataMemory()[blockCount - 1][4096 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[blockCount - 1][4096 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[blockCount - 1][4096 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(4096)], 2048);

    //Full data cache request / response
    cache.Request(5120, IType::St);
    ASSERT_TRUE(cache.isCacheMiss());
    ASSERT_EQ(cache.getWaitCycles(), cache.getFailLatency() + 120);
    ASSERT_EQ(cache.getRequestedIp(), ToLineAddr(5120));
    ASSERT_EQ(cache.getRequestedOffset(), ToLineOffset(5120));

    cache.setWaitCycles(0);
    responseData = 1111;
    ASSERT_TRUE(cache.Response(5120, responseData, IType::St, 20));
    ASSERT_FALSE(cache.getDataMemory()[0][5120 % (dataCacheBytes / blockCount) / lineSizeBytes].validityBit);
    ASSERT_EQ(cache.getDataMemory()[0][5120 % (dataCacheBytes / blockCount) / lineSizeBytes].lastUsage, 1);
    ASSERT_EQ(cache.getDataMemory()[0][5120 % (dataCacheBytes / blockCount) / lineSizeBytes].dataLine[ToLineOffset(5120)], 1111);

}

