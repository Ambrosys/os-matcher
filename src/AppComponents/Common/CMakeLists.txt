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

add_core_library( AppComponents ${SOURCE} )

find_package( Boost 1.76.0 REQUIRED )

target_link_libraries( AppComponents
    PUBLIC Core::Common
    PUBLIC Core::Graph
    PUBLIC Boost::boost
    PUBLIC CONAN_PKG::libpqxx
    PUBLIC pq
    )

target_include_directories( AppComponents
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
    )

install(
    TARGETS AppComponents
    ARCHIVE DESTINATION lib
)
