/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "IReader.h"

#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Point.h>

#include <Core/Common/Postgres/Connection.h>

#include <ambpipeline/Filter.h>

#include <unordered_set>

namespace AppComponents::Common::Reader {

/**
 * Reads OpenStreetMap data from a PostGIS database.
 */
class OsmMapReader : public IMapReader
{
public:
    OsmMapReader(
        Core::Common::Postgres::Connection & connection, std::unordered_set<Types::Street::HighwayType> const & highwaySelection, double fetchCorridor, bool useSingleSearchCircle);

    bool operator()(
        Types::Street::SegmentList &,
        Types::Street::NodePairList &,
        Types::Street::TravelDirectionList &,
        Types::Street::HighwayList &
    );

    bool operator()(
        Types::Track::PointList const &,
        Types::Street::SegmentList &,
        Types::Street::NodePairList &,
        Types::Street::TravelDirectionList &,
        Types::Street::HighwayList &
        );

    bool init(
        Types::Track::PointList const &,
        double fetchCorridor,
        bool useSingleSearchCircle,
        std::optional<std::unordered_set<Types::Street::HighwayType>> const & highwaySelection
        );

private:
    Core::Common::Postgres::Connection & connection_;
    std::unordered_set<Types::Street::HighwayType> highwaySelection_;
    double const fetchCorridor_;
    double searchRadius_;
    std::string pointsString_;
    bool const useSingleSearchCircle_;


};

}  // namespace AppComponents::Common::Reader
