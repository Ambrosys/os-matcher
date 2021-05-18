set( sources
    main.cpp
    graph_test.cpp
    routing_test.cpp
    )

add_core_test( UnitTestsGraph ${sources} )

target_link_libraries( UnitTestsGraph
    PUBLIC Core::Graph
    PUBLIC CONAN_PKG::catch2
    )

install(
    TARGETS UnitTestsGraph RUNTIME
    DESTINATION bin
)
