
/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Street/TravelDirection.h>

#include <Core/Common/Geometry/Types.h>

#include <cstddef>
#include <vector>

namespace AppComponents::Common::Types::Routing {

struct SamplingPointCandidate
{
    using Point = Core::Common::Geometry::Point;

    size_t streetIndex;
    size_t streetSegmentIndex;
    Point streetSegmentProjectedPoint;
    double streetSegmentProjectedPointNormLength;
    double streetSegmentDistance;
    double streetSegmentHeading;
    double streetSegmentHeadingDifference;
    Street::TravelDirection streetSegmentTravelDirection;
};

/**
 * Represents a projected track point.
 */
struct SamplingPoint
{
    size_t trackIndex;
    std::vector<SamplingPointCandidate> candidates;
};

using SamplingPointList = std::vector<SamplingPoint>;

}  // namespace AppComponents::Common::Types::Routing
