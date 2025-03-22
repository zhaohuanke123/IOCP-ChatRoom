cmake_minimum_required(VERSION 3.15)
project(IOCP_Server LANGUAGES CXX)

# 设置Windows编译选项
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)  # 目标Windows 10
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

get_directory_property(PARENT_VARS DIRECTORY .. VARIABLES PROTO_SOURCES)

add_executable(iocp_server main.cpp
    ${PARENT_VARS_PROTO_SOURCES}
)

add_dependencies(iocp_server person_proto)

# 链接Windows Socket库
target_link_libraries(iocp_server PRIVATE Ws2_32.lib
    ${PROTO_GEN_DIR}
)