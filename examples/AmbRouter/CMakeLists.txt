set( SOURCES
        main.cpp
        )

add_executable( AmbRouter ${SOURCES} )

target_link_libraries( AmbRouter
        PUBLIC Core::AppComponents
        PUBLIC amb-log
        PUBLIC amb-thread
        PUBLIC cli-app
        PUBLIC Progress
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