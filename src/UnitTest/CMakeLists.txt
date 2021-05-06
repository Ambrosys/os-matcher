# TODO: uncomment the following line when the CI uses cmake 19+
# cmake_policy( SET CMP0110 NEW )

set( SOURCES
    Helpers.cpp
    )

add_library( Helpers ${SOURCES} )
add_library( UnitTest::Helpers ALIAS Helpers )

target_link_libraries( Helpers
    PUBLIC CONAN_PKG::catch2
    PUBLIC CONAN_PKG::nlohmann_json
    PUBLIC Core::Common
    )

target_include_directories( Helpers
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )

add_subdirectory( Graph )
