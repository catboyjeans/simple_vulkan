cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS Y)
set(CMAKE_FIND_DEBUG_MODE TRUE)

project(vulkan_test)

set(TEST_SOURCES
    main.cpp
)

add_executable(vulkan_test ${TEST_SOURCES})

# Set up vulkan library
add_library(vulkan_lib UNKNOWN IMPORTED)
set_target_properties(vulkan_lib PROPERTIES
    IMPORTED_LOCATION /usr/local/lib/libvulkan.dylib
)
target_include_directories(vulkan_lib INTERFACE /usr/local/include)

target_link_libraries(vulkan_test PRIVATE vulkan_lib) 


