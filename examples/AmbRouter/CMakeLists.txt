set( SOURCES
        main.cpp
        )

add_executable( AmbRouter ${SOURCES} )
# TODO: Needed workaround to make the examples conan buildable
add_dependencies( AmbRouter test-if-git-HEAD-has-changed )

target_link_libraries( AmbRouter
        PUBLIC os-matcher
        #PUBLIC Core::AppComponents
        PUBLIC CONAN_PKG::amb-log
        PUBLIC CONAN_PKG::libpqxx
        PUBLIC CONAN_PKG::amb-thread
        PUBLIC CONAN_PKG::cli-app
        PUBLIC Progress
        #PUBLIC pq
        )

if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
        target_link_libraries( AmbRouter PRIVATE c++fs )
    endif()
else()
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1")
        target_link_libraries( AmbRouter PRIVATE stdc++fs )
    endif()
endif()

install(
        TARGETS AmbRouter RUNTIME
        DESTINATION bin
)