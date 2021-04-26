set( sources
    main.cpp
    ThreadPool_test.cpp
    DependencyGraph_test.cpp
    )

add_core_test( UnitTestsGeneric ${sources} )

target_link_libraries( UnitTestsGeneric
    PUBLIC CONAN_PKG::catch2
    PUBLIC amb-thread
    PUBLIC UnitTest::Helpers
    )
