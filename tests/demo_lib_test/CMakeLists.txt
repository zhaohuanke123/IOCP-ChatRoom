cmake_minimum_required(VERSION 3.14)
project(demo_lib_test)

# 生成测试可执行文件
add_executable(${PROJECT_NAME}
    test_demo_lib.cpp
    test_basic.cpp
    test_edge.cpp
)

# 配置依赖关系
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    test_utils
    net_lib
    GTest::gtest_main
)

# 注册测试用例
add_test(
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../test_data
)
