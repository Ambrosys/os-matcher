set( SOURCES
    Geometry/Types.cpp
    Geometry/Helper.cpp
    Geometry/Conversion.cpp
    Time/Helper.cpp
    )

add_object_core_library( Common ${SOURCES} )
target_compile_definitions(Common PUBLIC OBJ)

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

add_core_interface_library( CommonInterface )
target_sources( CommonInterface INTERFACE $<TARGET_OBJECTS:Common>)
target_include_directories(CommonInterface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<INSTALL_INTERFACE:include/Common>
        )

install(
    TARGETS Common CommonInterface
    ARCHIVE DESTINATION lib
)