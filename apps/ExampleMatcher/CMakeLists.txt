set( SOURCES
    main.cpp
    )

add_executable( ExampleMatcher ${SOURCES} )

target_link_libraries( ExampleMatcher
    PUBLIC Core::AppComponents
    PUBLIC amb-log
    PUBLIC amb-thread
    PUBLIC cli-app
    )

if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
        target_link_libraries( ExampleMatcher PRIVATE c++fs )
    endif()
else()
    if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1")
        target_link_libraries( ExampleMatcher PRIVATE stdc++fs )
    endif()
endif()

install(
    TARGETS ExampleMatcher RUNTIME
    DESTINATION bin
)
