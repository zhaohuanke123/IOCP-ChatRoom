file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)
add_executable(demo_app ${srcs})
target_link_libraries(demo_app PRIVATE Ws2_32.lib)