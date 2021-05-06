set( SOURCES
    LemonDigraph.cpp
    Routing/Dijkstra.cpp
    Routing/PathView.cpp
    )

add_core_library( Graph ${SOURCES} )

find_package( Boost 1.76.0 REQUIRED )

target_link_libraries( Graph
    PUBLIC Boost::boost
    )

target_include_directories( Graph
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )


install(
    TARGETS Graph ARCHIVE DESTINATION lib
)