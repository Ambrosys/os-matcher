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
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    target_link_libraries( ExampleMatcher PUBLIC c++experimental )
else()
    target_link_libraries( ExampleMatcher PUBLIC stdc++fs )
endif()

install(
    TARGETS ExampleMatcher RUNTIME
    DESTINATION bin
)
