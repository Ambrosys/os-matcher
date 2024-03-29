/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Common/Geometry/Helper.h>

#include <boost/geometry/strategies/spherical/distance_haversine.hpp>

namespace Core::Common::Geometry {

std::pair<Point, double> project(Point const & p, Segment const & segment)
{
    using boost::geometry::add_point;
    using boost::geometry::dot_product;
    using boost::geometry::multiply_value;
    using boost::geometry::subtract_point;

    auto const & p1 = segment.first;
    auto const & p2 = segment.second;
    auto p1p = p;
    subtract_point(p1p, p1);

    auto p1p2 = p2;
    subtract_point(p1p2, p1);

    const double segmentLength_sqred = dot_product(p1p2, p1p2);
    const double normLengthProjection = dot_product(p1p, p1p2) / segmentLength_sqred;

    auto projected = p1p2;
    multiply_value(projected, normLengthProjection);
    add_point(projected, p1);

    return {projected, normLengthProjection};
}

std::pair<Point, double> snap(Point const & p, Segment const segment)
{
    auto const result = project(p, segment);
    auto const & projected = result.first;
    auto const normLengthProjection = result.second;

    if (normLengthProjection <= 0.0)
    {
        return {segment.first, normLengthProjection};
    }
    else if (normLengthProjection >= 1.0)
    {
        return {segment.second, normLengthProjection};
    }
    else
    {
        return {projected, normLengthProjection};
    }
}

Point reverseHaversine(Point const & p, double bearing, double distanceMeter)
{
    using std::atan2;
    using std::cos;
    using std::sin;

    auto lon1 = rad(p.lon());
    auto lat1 = rad(p.lat());
    auto angdist = distanceMeter / equatorRadiusMeter;
    auto theta = rad(bearing);
    auto lat2 = asin(sin(lat1) * cos(angdist) + cos(lat1) * sin(angdist) * cos(theta));
    auto lon2 = lon1 + atan2(sin(theta) * sin(angdist) * cos(lat1), cos(angdist) - sin(lat1) * sin(lat2));
    return Point{}.setLon(degree(lon2)).setLat(degree(lat2));
}

Box buffer(Box const & box, double distanceMeter)
{
    constexpr auto sqrtOf2 = 1.414213562373095049;
    auto min_corner = reverseHaversine(box.min_corner(), 225.0, sqrtOf2 * distanceMeter);
    auto max_corner = reverseHaversine(box.max_corner(), 45.0, sqrtOf2 * distanceMeter);
    return {min_corner, max_corner};
}

ValueType geoDistance(Point const & g1, Point const & g2)
{
    auto strategy = boost::geometry::strategy::distance::haversine<double>(equatorRadiusMeter);
    return boost::geometry::distance(g1, g2, strategy);
}

std::tuple<ValueType, Point, double> geoDistance(Point const & g1, Segment const & g2)
{
    auto const clipped = snap(g1, g2);
    return {geoDistance(g1, clipped.first), clipped.first, clipped.second};
}

double heading(Point const & p1, Point const & p2)
{
    auto deltaLon = rad(p2.lon()) - rad(p1.lon());
    auto cos_p2lat = cos(rad(p2.lat()));
    auto angle = atan2(sin(deltaLon) * cos_p2lat, cos(rad(p1.lat())) * sin(rad(p2.lat())) - sin(rad(p1.lat())) * cos_p2lat * cos(deltaLon));
    return normalizeAngle(degree(angle));
}

double heading(Segment const & segment)
{
    return heading(segment.first, segment.second);
}

double normalizeAngle(double angle)
{
    angle = std::fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

double headingDiff(double a, double b)
{
    auto res = normalizeAngle(a - b);
    return res > 180.0 ? res - 360.0 : res;
}

double absHeadingDiff(double a, double b)
{
    return std::abs(headingDiff(a, b));
}

double reversedHeading(double heading)
{
    if (heading >= 180.0)
        return heading - 180.0;
    else
        return heading + 180.0;
}

LineStringProjectionResult projectOntoLineString(Point const & point, LineString const & lineString)
{
    assert(lineString.size() > 1);

    auto minSnapped = snap(point, Segment{lineString[0], lineString[1]});
    auto minDistance = geoDistance(point, minSnapped.first);
    size_t idx = 1;
    for (size_t i = 2; i < lineString.size(); ++i)
    {
        auto snapped = snap(point, Segment{lineString[i - 1u], lineString[i]});
        auto distance = geoDistance(point, snapped.first);

        if (distance < minDistance)
        {
            minSnapped = snapped;
            minDistance = distance;
            idx = i;
        }
    }

    auto result = LineStringProjectionResult{};
    result.projectionPoint = minSnapped.first;
    result.distanceToSegment = minDistance;
    result.segmentHeading = heading(Segment{lineString[idx - 1u], lineString[idx]});

    result.distanceAlongLineString = 0.0;
    for (size_t i = 1; i < idx; ++i)
    {
        result.distanceAlongLineString += geoDistance(lineString[i - 1u], lineString[i]);
    }
    result.distanceAlongLineString += geoDistance(lineString[idx - 1u], minSnapped.first);

    if (idx == 1 and minSnapped.second < 0.0)
    {
        result.offLineString = true;
    }
    else if (idx == lineString.size() - 1u and minSnapped.second > 1.0)
    {
        result.offLineString = true;
    }
    else
    {
        result.offLineString = false;
    }
    return result;
}

double relativeDistanceAlongLineString(LineString const & lineString, Point const & point)
{
    auto result = projectOntoLineString(point, lineString);
    return result.distanceAlongLineString / geoLength(lineString);
}

/**
 * Returns the angle between segment1 and segment2 in range [180°, -180°].
 */
double angleBetweenSegments(Segment const & segment1, Segment const & segment2)
{
    auto headingSeg1 = heading(segment1);
    auto headingSeg2 = heading(segment2);
    return headingDiff(headingSeg1, headingSeg2);
}

std::vector<Core::Common::Geometry::Point> trimmed(std::vector<Core::Common::Geometry::Point> const & points, double const trimLeft, double const trimRight)
{
    std::vector<Core::Common::Geometry::Point> newPoints;
    std::shared_ptr<Core::Common::Geometry::Point> newStart;
    std::shared_ptr<Core::Common::Geometry::Point> newEnd;
    size_t oldBegin = 0;
    size_t oldEnd = points.size();

    double a = 0.0;
    for (size_t i = 0; i < points.size() - 1; ++i)
    {
        a += Core::Common::Geometry::geoDistance(points[i], points[i + 1]);
        if (a >= trimLeft)
        {
            oldBegin = i + 1;
            if (a > trimLeft)
                newStart = std::make_shared<Core::Common::Geometry::Point>(
                    Core::Common::Geometry::reverseHaversine(points[i + 1], Core::Common::Geometry::heading(points[i + 1], points[i]), a - trimLeft));
            break;
        }
    }

    a = 0.0;
    for (size_t i = points.size() - 1; i > 0; --i)
    {
        a += Core::Common::Geometry::geoDistance(points[i], points[i - 1]);
        if (a >= trimRight)
        {
            oldEnd = i;
            if (a > trimRight)
            {
                newEnd = std::make_shared<Core::Common::Geometry::Point>(
                    Core::Common::Geometry::reverseHaversine(points[i - 1], Core::Common::Geometry::heading(points[i - 1], points[i]), a - trimRight));
            }
            break;
        }
    }

    if (newStart)
        newPoints.push_back(*newStart);
    newPoints.insert(newPoints.end(), points.begin() + oldBegin, points.begin() + oldEnd);
    if (newEnd)
        newPoints.push_back(*newEnd);

    return newPoints;
}

}  // namespace Core::Common::Geometry