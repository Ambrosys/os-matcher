/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/Routing/Helper.h>

#include <Core/Common/Geometry/Helper.h>

namespace AppComponents::Common::Matcher::Routing {

// TODO: Is Core::Common::Geometry::geoLength() a better alternative?
double geoDistance(Core::Common::Geometry::LineString lineString)
{
    double length = 0.0;
    for (size_t i = 0; i < lineString.size() - 1; ++i)
        length += Core::Common::Geometry::geoDistance(lineString[i], lineString[i + 1]);
    return length;
}

std::optional<std::shared_ptr<Types::Routing::Route>> findPreviousConnectedRoute(size_t startIndex, Types::Routing::RouteList const & routeList)
{
    if (routeList.empty())
        return std::nullopt;
    for (size_t i = routeList.size() - 1;; --i)
    {
        if (routeList[i]->target.samplingPoint.index == startIndex)
            return routeList[i];
        if (routeList[i]->target.samplingPoint.index < startIndex)
            break;
        if (i == 0)
            break;
    }
    return std::nullopt;
}

size_t attachToPreviousRoute(Types::Routing::RouteList & routeList, size_t sourceSamplingPointIndex)
{
    while (!routeList.empty() && routeList.back()->target.samplingPoint.index > sourceSamplingPointIndex)
        routeList.pop_back();
    return !routeList.empty() ? routeList.back()->target.samplingPoint.index : sourceSamplingPointIndex;
}

bool isSelfIntersectingRoute(Types::Routing::Route const & route)
{
    auto const & head = route.subRoutes.front().route.front();
    auto const & tail = route.subRoutes.back().route.back();
    for (size_t i = 0; i < route.subRoutes.size(); ++i)
        for (size_t j = 0; j < route.subRoutes[i].route.size(); ++j)
        {
            // check whether the head is contained in itself
            if (not(i == 0 && j == 0) && route.subRoutes[i].route[j] == head)
                return true;
            // check whether the tail is contained in itself
            if (not(i == route.subRoutes.size() - 1 && j == route.subRoutes[i].route.size() - 1) && route.subRoutes[i].route[j] == tail)
                return true;
        }
    return false;
}

bool checkMaxAngularDeviation(Types::Routing::Route const & route, double const maxAngularDeviation)
{
    using namespace Core::Common;
    std::vector<Geometry::Point> points;
    for (auto const & subRoute : route.subRoutes)
    {
        bool samePoint = not points.empty() && points.back() == subRoute.route.front();
        points.insert(points.end(), subRoute.route.begin() + (samePoint ? 1 : 0), subRoute.route.end());
    }
    if (points.size() < 3)
        return true;

    // Algorithm:
    // - For each angle in [0, 360]
    //     - Draw the corresponding line into a circle.
    //     - If we drawed into an empty circle or into the pie, do nothing.
    //     - Else fill out the smaller unfilled pie to prodcude a larger pie.
    //         - If the pie reaches half the circle, return false.
    // - Return true.

    double left = Geometry::heading(points[0], points[1]);
    double right = left;
    double before = left;

    for (size_t i = 1; i < points.size() - 1; ++i)
    {
        double angle = Geometry::normalizeAngle(Geometry::heading(points[i], points[i + 1]));
        double diff = Geometry::headingDiff(angle, before);

        // Only adjust `left` or `right` if `angle` lies in the empty space.
        // Rotate everything so that `rights` points to zero,
        // then between zero and the rotated `left` is the empty space.
        double angleRotated = Geometry::normalizeAngle(angle - right);
        double leftRotated = Geometry::normalizeAngle(left - right);
        if (angleRotated < (leftRotated == 0.0 ? 360.0 : leftRotated))
        {
            if (diff <= 0.0)
                left = angle;
            else
                right = angle;
        }

        if (Geometry::normalizeAngle(right + (360.0 - left)) > maxAngularDeviation)
            return false;

        before = angle;
    }

    return true;
}

double calcApproximateDistanceBetweenSamplingPoints(size_t const sourceSamplingPoint, size_t const targetSamplingPoint, Types::Routing::SamplingPointList const & samplingPointList)
{
    return Core::Common::Geometry::geoDistance(
        samplingPointList.at(sourceSamplingPoint).candidates.front().streetSegmentProjectedPoint,
        samplingPointList.at(targetSamplingPoint).candidates.front().streetSegmentProjectedPoint);
}

}  // namespace AppComponents::Common::Matcher::Routing
