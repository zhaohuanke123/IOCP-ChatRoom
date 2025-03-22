#include "gtest/gtest.h"
#include "demo_lib.hpp"

TEST(AdditionTest, BasicAssertions) {
    for (int i = 0;i < 100000; i++) {
        EXPECT_EQ(demo::add(2, 3), 5);
        EXPECT_EQ(demo::add(-1, 1), 0);
    }
}

TEST(EdgeCases, ZeroHandling) {
    EXPECT_EQ(demo::add(0, 0), 0);
    EXPECT_EQ(demo::add(100, -100), 0);
}