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


# include(FindProtobuf)                 # 显式包含Protobuf支持
# find_package(Protobuf REQUIRED)        # 强制要求找到Protobuf
set(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/proto/protoc.exe)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH ON)
file(GLOB_RECURSE PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

add_custom_command(
    OUTPUT 
        ${CMAKE_BINARY_DIR}/generated/person.pb.cc
        ${CMAKE_BINARY_DIR}/generated/person.pb.h
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}  
        --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
        --cpp_out=${CMAKE_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/person.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/person.proto
    COMMENT "[Proto] Generating person.pb.cc/h"
    VERBATIM
)

# (PROTO_SRCS PROTO_HDRS
#     ${PROTO_FILES}
#     PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated
# )

# add_library(person_proto STATIC
#     ${PROTO_SRCS}
#     ${PROTO_HDRS}
# )

# target_include_directories(person_proto PUBLIC
#     ${CMAKE_CURRENT_BINARY_DIR}/generated  # 生成头文件路径
#     ${CMAKE_CURRENT_SOURCE_DIR}/proto/include  # 假设protobuf头文件在此
# )

set(PROTO_SOURCES ${PROTO_GEN_DIR}/person.pb.cc PARENT_SCOPE)
set(PROTO_HEADERS ${PROTO_GEN_DIR}/person.pb.h PARENT_SCOPE)