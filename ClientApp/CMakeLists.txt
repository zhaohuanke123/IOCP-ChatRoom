file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(ClientApp ${srcs})
target_link_libraries(ClientApp PRIVATE net_lib Ws2_32.lib)