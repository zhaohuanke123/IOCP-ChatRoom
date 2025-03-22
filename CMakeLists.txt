# CMakeDemo/CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(CMakeDemo LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子目录（顺序无关）
add_subdirectory(libs/net_lib)
add_subdirectory(apps/demo_app)
add_subdirectory(apps/ServerApp)
add_subdirectory(apps/ClientApp)
