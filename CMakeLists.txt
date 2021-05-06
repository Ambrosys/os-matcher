cmake_minimum_required( VERSION 3.7 )

option ( CMAKE_DEBUG_OUTPUT "Enable if you want additional output for cmake's configure stage" OFF )
option ( BUILD_LIBRARY "Enable if you want to build the library (and applications)" ON )
option ( BUILD_APPLICATIONS "Enable if you want to build applications" ON )
option ( BUILD_DOCS "Enable if you want to build docs" OFF )

if ( ${CMAKE_DEBUG_OUTPUT} )
    set(CMAKE_DEBUG_TARGET_PROPERTIES
        INCLUDE_DIRECTORIES
        COMPILE_DEFINITIONS
        CONTAINER_SIZE_REQUIRED
        LIB_VERSION
    )
endif ()

include( "CMakeModules/CustomTargets.cmake" )
include( "CMakeModules/CustomBuildTypes.cmake" )
include( "CMakeModules/enable_cpp17.cmake" )
include( "CMakeModules/CreateVersionFile.cmake" )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/Global" )

project( OS-Matcher LANGUAGES C CXX ) # The version information is pulled from hte git tag.

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    add_compile_options( -stdlib=libc++ )  # Due to https://bugs.llvm.org/show_bug.cgi?id=33222
endif()

enable_cpp17()
add_compile_options( -Wall -Wextra  )

set ( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

if ( ${BUILD_LIBRARY} )
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    conan_cmake_run(
        REQUIRES

            # Ambrosys libraries
            amb-log/0.5.0@amb/stable
            amb-pipeline/0.4.0@amb/stable
            cli-app/0.4.0@amb/stable

            # A modern, C++-native, header-only, framework for unit-tests.
            catch2/2.13.6

            # JSON for Modern C++.
            nlohmann_json/3.9.1

            # Boost C++ libraries.
            boost/1.76.0

            libpqxx/7.5.0


        BASIC_SETUP
        NO_OUTPUT_DIRS
        PROFILE_AUTO
        CMAKE_TARGETS
        BUILD missing
    )

    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()

    include_directories( ${CMAKE_BINARY_DIR}/GeneratedIncludes )
    include_directories( ThirdParty/install/include )
    link_directories( ThirdParty/install/lib )

    add_compile_options( -D__CMAKE_BINARY_DIR__=\"${CMAKE_BINARY_DIR}\" )
    add_compile_options( -D__CMAKE_SOURCE_DIR__=\"${CMAKE_SOURCE_DIR}\" )

    add_subdirectory( src )

    if ( ${BUILD_APPLICATIONS} )
        add_subdirectory( apps )
    endif ()
endif ()

if ( ${BUILD_DOCS} )
    add_subdirectory( docs )
endif ()
