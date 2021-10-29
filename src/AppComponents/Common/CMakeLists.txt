set( SOURCE
    Reader/CsvTrackReader.cpp
    Reader/GeoJsonMapReader.cpp
    Reader/JsonTrackReader.cpp
    Reader/Osm/Conversion.cpp
    Reader/OsmMapReader.cpp

    Matcher/Router.cpp
    Matcher/Routing/DirectedCandidateRouter.cpp
    Matcher/Routing/SamplingPointRouter.cpp
    Matcher/Routing/BacktrackRouter.cpp
    Matcher/Routing/SkipRouter.cpp
    Matcher/Routing/PiecewiseRouter.cpp
    Matcher/Routing/Comparators.cpp
    Matcher/Routing/Helper.cpp
    Matcher/Routing/Generic/Helper.cpp
    Matcher/Routing/Generic/Skipper.cpp
    Matcher/GraphBuilder.cpp
    Matcher/SamplingPointFinder.cpp

    Writer/CsvRouteWriter.cpp
    Writer/CsvSubRouteWriter.cpp
    Writer/GeoJsonRouteWriter.cpp
    Writer/GeoJsonTrackWriter.cpp
    Writer/GeoJsonMapWriter.cpp
    Writer/JsonRouteStatisticWriter.cpp
    )

add_core_library( AppComponents $<TARGET_OBJECTS:Graph> $<TARGET_OBJECTS:Common> ${SOURCE} )
# ToDo: In the order of working on os-matcher-amb#78 ('conan package creation)
#       an object library of object library is kind of possible but definitions could not successfully be linked
#       (see https://cmake.org/cmake/help/v3.13/command/target_link_libraries.html?highlight=target_link_libraries#linking-object-libraries)
#       If a solution can be found, the os-matcher lib in the header CMakeLists.txt would become far easier
#add_object_core_library( AppComponents $<TARGET_OBJECTS:Graph> $<TARGET_OBJECTS:Common> ${SOURCE} )
#target_compile_definitions(AppComponents PUBLIC OBJ)

find_package( Boost 1.76.0 REQUIRED )

target_link_libraries( AppComponents
    PUBLIC Core::Common
    PUBLIC Core::Graph
    PUBLIC Boost::boost
    PUBLIC CONAN_PKG::amb-pipeline
    PUBLIC CONAN_PKG::libpqxx
    PUBLIC CONAN_PKG::nlohmann_json
    #PUBLIC pq
    )

target_include_directories( AppComponents
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>/include
    )

install(
    TARGETS AppComponents
    ARCHIVE DESTINATION lib
)
