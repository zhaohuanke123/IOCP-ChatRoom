#pragma once
#include <gtest/gtest.h>

// 自定义断言宏
#define EXPECT_IN_RANGE(VAL, MIN, MAX) \
    EXPECT_GE((VAL), (MIN));           \
    EXPECT_LE((VAL), (MAX))

// 测试夹具基类
class TimerFixture : public ::testing::Test {
protected:
    void SetUp() override {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void TearDown() override {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::cout << "测试耗时: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time).count()
                  << "ms" << std::endl;
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};
