//
// Created by friedrich on 16.10.21.
//

#ifndef OS_MATCHER_IWRITER_H
#define OS_MATCHER_IWRITER_H

#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <ostream>
namespace AppComponents::Common::Writer{


struct IRouteWriter {

    /**
     * Writes the route to the output stream
     *
     * Needed for route geometry
     * - RouteList
     * - GraphEdgeMap
     * - SegmentList
     *
     * @return success status
     */
    virtual bool operator()(
        std::ostream &,
        Types::Routing::RouteList const &,
        Types::Graph::GraphEdgeMap const &,
        Types::Street::SegmentList const &,
        Types::Graph::NodeMap const &,
        Types::Track::TimeList const &,
        Types::Routing::SamplingPointList const &
        )=0;

};

struct ITrackWriter {

    /**
     * Writes track back from the four-fold representation of Tracks:
     *  - TimeList: List of timestamps
     *  - PointList: List of GeoPositions per timestamp
     *  - HeadingList: List of directions per timestamp
     *  - VelocityList: List of velocities per timestamp.
     *
     *  In general:
     *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
     *
     * @return success status
     */
    virtual bool operator()(
        std::ostream &,
        Types::Track::TimeList const &,
        Types::Track::PointList const &,
        Types::Track::HeadingList const &,
        Types::Track::VelocityList const &
        )=0;

};

struct IMapWriter {

    /**
     * Writes map back from the four-fold representation of streetmaps
     *  - SegmentList: List of street segments from the map
     *  - NodePairList: List of junction pairs, in which a segment lies in between
     *  - TravelDirectionList: List of allowed travel directions in orientation from the first node to the second node of the corresponding NodePair
     *  - HighwayList: List of street types for the corresponding street segment
     *
     *  In general:
     *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
     *
     * @return
     */
    virtual bool operator()(
        std::ostream &,
        Types::Street::SegmentList const &,
        Types::Street::NodePairList const &,
        Types::Street::TravelDirectionList const &,
        Types::Street::HighwayList const &
        )=0;

};

struct IRouteStatisticWriter {
    /**
     *
     */
     virtual bool operator() (
        std::ostream &,
        Types::Routing::RoutingStatistic const &,
        Types::Routing::SamplingPointList const &,
        Types::Track::TimeList const &
        ) = 0;
};
} // namespace AppComponents::Common::Writer

#endif  //OS_MATCHER_IWRITER_H
