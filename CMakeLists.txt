cmake_minimum_required( VERSION 3.13 )
project( os-matcher LANGUAGES C CXX )

option ( CMAKE_DEBUG_OUTPUT "Enable if you want additional output for cmake's configure stage" OFF )
option ( BUILD_LIBRARY "Enable if you want to build the library (and applications)" ON )
option ( BUILD_EXAMPLES "Enable if you want to build example applications" OFF )
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
include( "CMakeModules/FindTool.cmake" )
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/Global" )

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    add_compile_options( -stdlib=libc++ )  # Due to https://bugs.llvm.org/show_bug.cgi?id=33222
endif()
enable_cpp17()
add_compile_options( -Wall -Wextra )

if ( ${BUILD_LIBRARY} )

    list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})


    include( ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake )
    conan_basic_setup( TARGETS )


    find_package( Boost 1.76.0 REQUIRED )
    # To find lemon is a science in itself
    find_package( lemon 1.3.2 REQUIRED )

    include_directories( ${CMAKE_BINARY_DIR}/GeneratedIncludes )
    include_directories( ${CONAN_INCLUDE_DIRS} )

    enable_cpp17()
    add_compile_options( -D__CMAKE_BINARY_DIR__=\"${CMAKE_BINARY_DIR}\" )
    add_compile_options( -D__CMAKE_SOURCE_DIR__=\"${CMAKE_SOURCE_DIR}\" )

    add_subdirectory( src )

    set( SOURCE
            ${CMAKE_CURRENT_SOURCE_DIR}/src/OsMatcher.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Reader/CsvTrackReader.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Reader/GeoJsonMapReader.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Reader/JsonTrackReader.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Reader/Osm/Conversion.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Reader/OsmMapReader.cpp

            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Matcher/Router.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Matcher/GraphBuilder.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Matcher/SamplingPointFinder.cpp

            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/CsvRouteWriter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/CsvSubRouteWriter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/GeoJsonRouteWriter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/GeoJsonTrackWriter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/GeoJsonMapWriter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/AppComponents/Common/Writer/JsonRouteStatisticWriter.cpp
            )

    add_library( os-matcher STATIC $<TARGET_OBJECTS:Common> $<TARGET_OBJECTS:Graph> ${SOURCE} )

    target_link_libraries( os-matcher
            PUBLIC Core::Common
            PUBLIC Core::Graph
            PUBLIC Boost::boost
            PUBLIC CONAN_PKG::amb-pipeline
            PUBLIC CONAN_PKG::libpqxx
            PUBLIC CONAN_PKG::nlohmann_json
            PUBLIC pq
            )

    target_include_directories( os-matcher
            PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            )

    install(
            TARGETS os-matcher
            ARCHIVE DESTINATION lib
    )

    if ( ${BUILD_EXAMPLES} )
        add_subdirectory( examples )
    endif ()
endif ()

if ( ${BUILD_DOCS} )
    add_subdirectory( docs )
endif ()