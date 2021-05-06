set( SOURCES
    Geometry/Types.cpp
    Geometry/Helper.cpp
    Geometry/Conversion.cpp
    Time/Helper.cpp
    )

add_core_library( Common ${SOURCES} )


set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )
find_package( Boost 1.76.0 REQUIRED )
target_link_libraries( Common
    PUBLIC Boost::boost
    PUBLIC CONAN_PKG::nlohmann_json
    )

target_include_directories( Common
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )


install(
    TARGETS Common
    ARCHIVE DESTINATION lib
)