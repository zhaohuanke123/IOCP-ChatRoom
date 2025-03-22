#include "test_helpers.hpp"
#include "demo_lib.hpp"

TEST(ArithmeticTest, BasicAddition) {
    EXPECT_EQ(demo::add(2, 3), 5);    // 正常值测试
    EXPECT_NE(demo::add(1, 1), 3);    // 反向验证
}

// TEST(StringTest, Concatenation) {
//     auto result = demo::concat("Hello", "World");
//     EXPECT_STREQ(result.c_str(), "HelloWorld");  // 字符串比较
// }
