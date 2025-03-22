project(net_lib)

# 自动收集所有C++源文件和头文件
file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB HEADERS CONFIGURE_DEPENDS "*.hpp")

# 设置Windows编译选项
if(WIN32)
    add_compile_definitions(
        _WIN32_WINNT=0x0A00
        # 移除_WINSOCKAPI_定义（由Winsock2.h自动处理）
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 生成静态库
add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}  # 可选：帮助IDE识别头文件关联性
)

# 头文件目录配置（保持不变）
target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
