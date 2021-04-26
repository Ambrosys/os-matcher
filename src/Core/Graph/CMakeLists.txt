set( SOURCES
    LemonDigraph.cpp
    Routing/Dijkstra.cpp
    Routing/PathView.cpp
    )

add_core_library( Graph ${SOURCES} )

target_link_libraries( Graph
    PUBLIC CONAN_PKG::boost_iterator
    )

target_include_directories( Graph
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )


install(
    TARGETS Graph ARCHIVE DESTINATION lib
)