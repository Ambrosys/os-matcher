
#ifndef OS_MATCHER_IREADER_H
#define OS_MATCHER_IREADER_H

#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <unordered_set>


namespace AppComponents::Common::Reader{

struct ITrackReader
{
    virtual ~ITrackReader() {}

    /**
     * Fills the provided datastructures with the four-fold representation of Tracks:
     *  - TimeList: List of timestamps
     *  - PointList: List of GeoPositions per timestamp
     *  - HeadingList: List of directions per timestamp
     *  - VelocityList: List of velocities per timestamp
     *
     *  In general:
     *      len(TimeList) = len(PointList) = len(HeadingList) = len(VelocityList)
     *
     * @return success status
     */
    virtual bool operator()(
        Types::Track::TimeList &,
        Types::Track::PointList &,
        Types::Track::HeadingList &,
        Types::Track::VelocityList &
        ) = 0;
};

struct IMapReader
{
    virtual ~IMapReader() {}
    /**
     * Fills the provided datastructures with the four-fold representation of streetmaps
     *  - SegmentList: List of street segments from the map
     *  - NodePairList: List of junction pairs, in which a segment lies in between
     *  - TravelDirectionList: List of allowed travel directions in orientation from the first node to the second node of the corresponding NodePair
     *  - HighwayList: List of street types for the corresponding street segment
     *
     *  In general:
     *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
     *
     * @return success status
     */
    virtual bool operator()(
        Types::Street::SegmentList &,
        Types::Street::NodePairList &,
        Types::Street::TravelDirectionList &,
        Types::Street::HighwayList &) = 0;

    /**
     * Initialize IMapReader to only read map data along the PointList
     * @return
     */
    virtual bool init(
        Types::Track::PointList const &,
        double fetchCorridor,
        bool useSingleSearchCircle,
        std::optional<std::unordered_set<Types::Street::HighwayType>> const &
        ){ return false; };

};

}

#endif  //OS_MATCHER_IREADER_H
