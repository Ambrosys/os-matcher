set( SOURCES
    main.cpp
    Filter/CsvSubRouteWriter.cpp
    Filter/CsvRouteWriter.cpp
    Filter/GeoJsonRouteWriter.cpp
    )

add_executable( ExampleMatcher ${SOURCES} )

target_link_libraries( ExampleMatcher
    PUBLIC Core::AppComponentsExampleMatcher
    PUBLIC Core::AppComponents
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
