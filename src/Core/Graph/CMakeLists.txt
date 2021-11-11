set( SOURCES
    LemonDigraph.cpp
    Routing/Dijkstra.cpp
    Routing/PathView.cpp
    )

add_object_core_library( Graph ${SOURCES} )
target_compile_definitions(Graph PUBLIC OBJ)

find_package( Boost 1.76.0 REQUIRED )
find_package( lemon 1.3.2 REQUIRED )

target_link_libraries( Graph
    PUBLIC Boost::boost
    PUBLIC CONAN_PKG::lemon
    )

target_include_directories( Graph
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )

add_core_interface_library( GraphInterface )
target_sources( GraphInterface INTERFACE $<TARGET_OBJECTS:Graph>)
target_include_directories(GraphInterface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:include/Graph>
        )

install(
    TARGETS Graph GraphInterface ARCHIVE DESTINATION lib
)