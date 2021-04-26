set( SOURCE
    Filter/CsvTrackReader.cpp
    Filter/JsonTrackReader.cpp
    )

# TODO: find better solution, this is no Core library
add_core_library( AppComponentsExampleMatcher ${SOURCE} )

target_link_libraries( AppComponentsExampleMatcher
    PUBLIC Core::Common
    PUBLIC Core::AppComponents
    PUBLIC CONAN_PKG::amb-log
    PUBLIC CONAN_PKG::nlohmann_json
    )

target_include_directories( AppComponentsExampleMatcher
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )

install(
    TARGETS AppComponentsExampleMatcher
    ARCHIVE DESTINATION lib
)