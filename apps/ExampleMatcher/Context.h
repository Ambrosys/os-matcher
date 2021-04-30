/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Graph/LemonDigraph.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Altitude.h>
#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

struct Context
{
    struct
    {
        AppComponents::Common::Types::Track::TimeList timeList;
        AppComponents::Common::Types::Track::PointList pointList;
        AppComponents::Common::Types::Track::AltitudeList altitudeList;
        AppComponents::Common::Types::Track::HeadingList headingList;
        AppComponents::Common::Types::Track::VelocityList velocityList;
    } track;

    struct
    {
        AppComponents::Common::Types::Street::SegmentList segmentList;
        AppComponents::Common::Types::Street::NodePairList nodePairList;
        AppComponents::Common::Types::Street::TravelDirectionList travelDirectionList;
        AppComponents::Common::Types::Street::HighwayList highwayList;
    } street;

    struct
    {
        AppComponents::Common::Types::Graph::LemonDigraph lemonDigraph;
        AppComponents::Common::Types::Graph::GraphEdgeMap graphEdgeMap;
        AppComponents::Common::Types::Graph::StreetIndexMap streetIndexMap;
        AppComponents::Common::Types::Graph::NodeMap nodeMap;
    } graph;

    struct
    {
        AppComponents::Common::Types::Routing::RouteList routeList;
        AppComponents::Common::Types::Routing::SamplingPointList samplingPointList;
        AppComponents::Common::Types::Routing::RoutingStatistic routingStatistic;
    } routing;
};
