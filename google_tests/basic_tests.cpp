//
// Created by shevchenya on 15/11/2020.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/Memory.h"

TEST(BasicFinctionsTests, testToWordAddrFunction) {
    ASSERT_EQ(ToWordAddr(2064), 516);
    ASSERT_EQ(ToWordAddr(0), 0);
    ASSERT_EQ(ToWordAddr(24), 6);
    ASSERT_EQ(ToWordAddr(-12), 1073741821);
    ASSERT_EQ(ToWordAddr(1024), 256);
    ASSERT_EQ(ToWordAddr(516), 129);
    ASSERT_EQ(ToWordAddr(257), 64);
}


TEST(BasicFinctionsTests, testToLineAddrFinction) {
    ASSERT_EQ(ToLineAddr(512), 512);
    ASSERT_EQ(ToLineAddr(488), 384);
    ASSERT_EQ(ToLineAddr(1024), 1024);
    ASSERT_EQ(ToLineAddr(516), 512);
    ASSERT_EQ(ToLineAddr(256), 256);
    ASSERT_EQ(ToLineAddr(811), 768);
    ASSERT_EQ(ToLineAddr(204), 128);
    ASSERT_EQ(ToLineAddr(2052), 2048);
    ASSERT_EQ(ToLineAddr(532), 512);


    for (int i = 0; i < 15; i++)
    {
        ASSERT_EQ(ToLineAddr(512 + rand() % 127), 512);
    }
}

TEST(BasicFinctionsTests, testToLineOffset) {
    ASSERT_EQ(ToLineOffset(512), 0);
    ASSERT_EQ(ToLineOffset(516), 1);
    ASSERT_EQ(ToLineOffset(204), 19);
    ASSERT_EQ(ToLineOffset(525), 3);
    ASSERT_EQ(ToLineOffset(1008), 28);
    ASSERT_EQ(ToLineOffset(202), 18);
    ASSERT_EQ(ToLineOffset(508), 31);

    for (int i = 0; i < 32; i++)
    {
        ASSERT_EQ(ToLineOffset(128 + 4 * i), i);
    }
}