cmake_minimum_required( VERSION 3.7 )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/Global" )

project( OS-Matcher LANGUAGES C CXX )

add_compile_options( -Wall -Wextra  )  # Set strict compiler options.
add_compile_options( -fdiagnostics-color=always )

set ( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

add_subdirectory( src )

option ( BUILD_DOCS "Enable if you want to build Docs" OFF )
if ( ${BUILD_DOCS} )
    add_subdirectory( docs )
endif ()
