set( SOURCES
    main.cpp
    foo.cpp
    )

add_executable ( OsMatcher ${SOURCES} )

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    target_link_libraries( OsMatcher PUBLIC c++experimental )
else()
    target_link_libraries( OsMatcher PUBLIC stdc++fs )
endif()

install(
    TARGETS OsMatcher RUNTIME
    DESTINATION bin
)
