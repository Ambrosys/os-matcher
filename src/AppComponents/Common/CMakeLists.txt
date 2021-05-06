set( SOURCE
    Filter/GeoJsonTrackWriter.cpp
    Filter/GeoJsonMapWriter.cpp
    Filter/GeoJsonMapReader.cpp
    Filter/OsmMapReader.cpp
    Filter/Osm/Conversion.cpp
    Filter/Router.cpp
    Filter/Routing/DirectedCandidateRouter.cpp
    Filter/Routing/SamplingPointRouter.cpp
    Filter/Routing/BacktrackRouter.cpp
    Filter/Routing/SkipRouter.cpp
    Filter/Routing/PiecewiseRouter.cpp
    Filter/Routing/Comparators.cpp
    Filter/Routing/Helper.cpp
    Filter/Routing/Generic/Helper.cpp
    Filter/Routing/Generic/Skipper.cpp
    Filter/GraphBuilder.cpp
    Filter/SamplingPointFinder.cpp
    Filter/JsonRouteStatisticWriter.cpp
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
