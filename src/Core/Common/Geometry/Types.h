#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>

namespace Core::Common::Geometry {

/// Numeric basis.
using ValueType = double;

struct Point
{
public:
    struct Latitude;
    struct Longitude;

    Point();
    explicit Point(Longitude lon, Latitude lat);
    explicit Point(Latitude lat, Longitude lon);

    Point(Point const &) = default;
    Point(Point &&) = default;

    Point & operator=(Point const &) = default;
    Point & operator=(Point &&) = default;

    ~Point() = default;

    ValueType lat() const;
    ValueType lon() const;

    Point & setLat(ValueType latitude);
    Point & setLon(ValueType longitude);

    std::string to_string() const;

private:
    ValueType longitude_;
    ValueType latitude_;
};

struct Point::Latitude
{
    explicit Latitude(ValueType value);
    Latitude operator-() const;
    explicit operator ValueType() const;

private:
    ValueType value;
};

struct Point::Longitude
{
    explicit Longitude(ValueType value);
    Longitude operator-() const;
    explicit operator ValueType() const;

private:
    ValueType value;
};

inline bool operator==(Point const & a, Point const & b)
{
    return a.lat() == b.lat() && a.lon() == b.lon();
}

Point::Latitude operator"" _lat(long double a);

Point::Longitude operator"" _lon(long double a);

/// WGS84 in degrees.
using CoordinateSystem = boost::geometry::cs::spherical_equatorial<boost::geometry::degree>;

/// Boxtype. The sides of the box are parallel to the coordinate system axis.
using Box = boost::geometry::model::box<Point>;

/// Segment type.
using Segment = boost::geometry::model::segment<Point>;

/// Line string type.
using LineString = boost::geometry::model::linestring<Point>;

/// Polygontyp. A polygon cannot have holes.
using Polygon = boost::geometry::model::ring<Point>;

}  // namespace Core::Common::Geometry

BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(Core::Common::Geometry::Point, Core::Common::Geometry::ValueType, Core::Common::Geometry::CoordinateSystem, lon, lat, setLon, setLat)
