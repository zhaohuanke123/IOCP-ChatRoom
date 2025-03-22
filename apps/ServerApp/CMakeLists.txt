cmake_minimum_required(VERSION 3.15)
project(Server LANGUAGES CXX)

# 设置Windows编译选项
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)  # 目标Windows 10
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_executable(Server main.cpp)

# 链接Windows Socket库
target_link_libraries(Server PRIVATE net_lib Ws2_32.lib)