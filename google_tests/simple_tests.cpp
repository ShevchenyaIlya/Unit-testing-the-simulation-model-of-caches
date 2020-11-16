//
// Created by shevchenya on 14/11/2020.
//

#import "gtest/gtest.h"
#include "../src/Memory.h"
#include "../src/Instruction.h"

int sum(int num1, int num2) {
    return num1 + num2;
}

TEST(tests, tests_sum_eq) {
    ASSERT_EQ(4, sum(2, 2));
}


TEST(tests, tests_sum_neq) {
    ASSERT_NE(3, sum(2, 2));
}


//TODO: Rerun all prev created tests
TEST(additional, test_implicit) {
    MemoryStorage mem ;
    mem.LoadElf("/home/shevchenya/CLionProjects/CourseWorkCache/programs/build/assembly/bin/simple.riscv");
    UncachedMem uncachedMem = UncachedMem (mem);
    std::unique_ptr<CachedMem> memModelPtr( new CachedMem(uncachedMem));
}