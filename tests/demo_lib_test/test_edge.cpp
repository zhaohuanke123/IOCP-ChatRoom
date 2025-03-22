#include "test_helpers.hpp"
#include "demo_lib.hpp"

TEST(EdgeCaseTest, ZeroHandling) {
    EXPECT_EQ(demo::add(0, 0), 0);       // 零值边界
    EXPECT_EQ(demo::add(INT_MAX, 1), INT_MIN);  // 整数溢出
}

// TEST(ErrorTest, InvalidInput) {
//     EXPECT_THROW({
//         demo::parse_input("invalid");    // 异常测试
//     }, std::invalid_argument);
// }
