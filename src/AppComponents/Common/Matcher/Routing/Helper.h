/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>

#include <Core/Common/Geometry/Types.h>

#include <functional>
#include <optional>
#include <unordered_set>

namespace AppComponents::Common::Matcher::Routing {

enum class RouteResult { goalReached, goalNotReachedButReachable, goalNotReachable };

// TODO: only for debugging
inline std::string routeResultToString(RouteResult const result)
{
    switch (result)
    {
        case RouteResult::goalReached: return "goalReached";
        case RouteResult::goalNotReachedButReachable: return "goalNotReachedButReachable";
        case RouteResult::goalNotReachable: return "goalNotReachable";
    }
    return std::to_string(static_cast<size_t>(result));
}

double geoDistance(Core::Common::Geometry::LineString lineString);

std::optional<std::shared_ptr<Types::Routing::Route>> findPreviousConnectedRoute(size_t startIndex, Types::Routing::RouteList const & routeList);

size_t attachToPreviousRoute(Types::Routing::RouteList & routeList, size_t sourceSamplingPointIndex);

bool isSelfIntersectingRoute(Types::Routing::Route const & route);

bool checkMaxAngularDeviation(Types::Routing::Route const & route, double maxAngularDeviation);

double
calcApproximateDistanceBetweenSamplingPoints(size_t const sourceSamplingPoint, size_t const targetSamplingPoint, Types::Routing::SamplingPointList const & samplingPointList);

}  // namespace AppComponents::Common::Matcher::Routing
