cmake_minimum_required(VERSION 3.14)
project(UnitTests LANGUAGES CXX)

# 启用测试框架
enable_testing()

# 配置覆盖率（可选）
option(ENABLE_COVERAGE "生成测试覆盖率报告" ON)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    link_libraries(--coverage)
endif()

# 添加公共测试工具库
add_library(test_utils STATIC
    utils/mock_functions.cpp
)
target_include_directories(test_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)
target_link_libraries(test_utils PRIVATE 
    GTest::gtest
    net_lib  # 被测库
)

# 包含子测试目录
add_subdirectory(demo_lib_test)

# 覆盖率报告目标
if(ENABLE_COVERAGE)
    add_custom_target(coverage
        COMMAND lcov --capture --directory . --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "生成覆盖率报告"
    )
endif()