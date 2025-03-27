file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp include/*.h)
add_library(net_lib STATIC ${srcs})
target_include_directories(net_lib PUBLIC include)
