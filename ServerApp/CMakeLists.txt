file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(ServerApp ${srcs})
target_link_libraries(ServerApp PRIVATE net_lib Ws2_32.lib)