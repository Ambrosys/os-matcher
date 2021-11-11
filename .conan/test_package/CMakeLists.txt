cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

include( ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake )
conan_basic_setup( TARGETS )

set( CMAKE_CXX_STANDARD 17 )
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

find_package(os-matcher 0.1 REQUIRED)

add_executable(example src/example.cpp)
target_link_libraries(example
        PUBLIC CONAN_PKG::os-matcher
        )
