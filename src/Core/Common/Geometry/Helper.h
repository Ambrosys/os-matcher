/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Common/Geometry/NumericConstants.h>
#include <Core/Common/Geometry/Types.h>

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/length.hpp>
#include <boost/geometry/algorithms/within.hpp>

#include <cmath>
#include <utility>

namespace Core::Common::Geometry {

struct LineStringProjectionResult
{
    Point projectionPoint;
    double distanceToSegment;        ///< in meters.
    double distanceAlongLineString;  ///< in meters.
    double segmentHeading;           ///< azimuth in degrees.
    bool offLineString;              ///< has no normal projection onto segment.
};

/// Project Point onto Segment.
/// \return Projection point and distance along segment normalized to segment
///         length.
std::pair<Point, double> project(Point const & p, Segment const & segment);

/// Clip point p to segment.
/// \return first: closest point to p on segment, second: distance along segment
///         as returned by project().
std::pair<Point, double> snap(Point const & p, Segment segment);

/// Implementation of the reverse of Haversine formula.
/// \return Point with \p bearing and \p distanceMeter away from point \p p.
Point reverseHaversine(Point const & p, double bearing, double distanceMeter);

/// Enlarge a box by \p distanceMeter on all sides.
Box buffer(Box const & box, double distanceMeter);

/// Calculate length of geometry \p g1 on earth surface.
/// \return length in meters.
template <typename G1>
auto geoLength(G1 const & g1)
{
    return boost::geometry::length(g1) * equatorRadiusMeter;
}

/// Calculate distance between geometries \p g1 and \p g2 on earth surface.
/// \return distance in meters.
template <typename G1, typename G2>
ValueType geoDistance(G1 const & g1, G2 const & g2)
{
    return boost::geometry::distance(g1, g2) * equatorRadiusMeter;
}

ValueType geoDistance(Point const & g1, Point const & g2);

/// \return { distance, projectedPointOnSegment, normLengthOnSegment }
std::tuple<ValueType, Point, double> geoDistance(Point const & g1, Segment const & g2);

/// Calculate the initial heading for a great-circle route from point \p p1 to
/// point \p p2.
/// \return heading in degrees.
double heading(Point const & p1, Point const & p2);

/// Calculate the heading between \p segment start and end point.
/// \return heading in degrees.
double heading(Segment const & segment);

/// Normalize \p angle, given in degrees, to the range [0°, 360°).
/// \return angle in degrees.
double normalizeAngle(double angle);

/// Calculate difference in heading normalized to the range (-180°, +180°].
/// \return angle difference in degree.
double headingDiff(double a, double b);

/// Calculate difference in heading normalized to the range [0°, +180°).
/// \return angle difference in degree.
double absHeadingDiff(double a, double b);

/// Reverses the given heading.
/// \param heading Has to be in the range [-180°, 540°).
/// \return Reversed heading in the range [0°, 360°).
double reversedHeading(double heading);

/// Convert from degree to radians.
/// \return angle in rad.
template <typename T>
constexpr auto rad(T degree)
{
    return degree * piover180;
}

/// Convert from radians to degree.
/// \return angle in deg.
template <typename T>
constexpr auto degree(T rad)
{
    return rad / piover180;
}

/**
 * Search for minimum distance of point to line in the lineString.
 *
 * A)
 *          p
 *          ^
 *          | <~~ distance to segment
 *          |
 * p1<======x---------->p2
 *          ^~~~projected point/distance along segment
 *
 * B)
 *                         p
 *                        /
 *                       / <~~ distance to segment
 *                      /
 * p1<================x>p2
 *                    ^~~~projected point/distance along segment
 */
LineStringProjectionResult projectOntoLineString(Point const & point, LineString const & lineString);

double relativeDistanceAlongLineString(LineString const & lineString, Point const & point);

double angleBetweenSegments(Segment const & segment1, Segment const & segment2);

template <typename Geometry1, typename Geometry2>
bool within(Geometry1 const & geometry1, Geometry2 const & geometry2)
{
    return boost::geometry::within(geometry1, geometry2);
}

std::vector<Core::Common::Geometry::Point> trimmed(std::vector<Core::Common::Geometry::Point> const & points, double trimLeft, double trimRight);

std::vector<Core::Common::Geometry::Point> flattened_simple(std::vector<Core::Common::Geometry::Point> const & points, size_t keepEachNthPoint);

}  // namespace Core::Common::Geometry
