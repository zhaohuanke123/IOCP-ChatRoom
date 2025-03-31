cmake_minimum_required(VERSION 3.18)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules;${CMAKE_MODULE_PATH}")

project(IOCP_CHATROOM LANGUAGES CXX)

include(MyUsefulFuncs)

if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0A00)  # 目标Windows 10
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 添加子目录（顺序无关）
add_subdirectory(net_lib)
add_subdirectory(demo_app)
add_subdirectory(ServerApp)
add_subdirectory(ClientApp)

# include(FindProtobuf)                 # 显式包含Protobuf支持
# find_package(Protobuf REQUIRED)        # 强制要求找到Protobuf
# set(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/proto/protoc.exe)
# set(PROTOBUF_GENERATE_CPP_APPEND_PATH ON)
# file(GLOB_RECURSE PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
