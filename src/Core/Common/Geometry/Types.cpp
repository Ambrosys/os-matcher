/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Common/Geometry/Types.h>

#include <type_traits>

namespace Core::Common::Geometry {

Point::Point() : longitude_(ValueType(0)), latitude_(ValueType(0))
{
}

Point::Point(Longitude const lon, Latitude const lat) : longitude_(lon), latitude_(lat)
{
}

Point::Point(Latitude const lat, Longitude const lon) : longitude_(lon), latitude_(lat)
{
}

ValueType Point::lat() const
{
    return latitude_;
}

ValueType Point::lon() const
{
    return longitude_;
}

Point & Point::setLat(ValueType const latitude)
{
    latitude_ = latitude;
    return *this;
}

Point & Point::setLon(ValueType const longitude)
{
    longitude_ = longitude;
    return *this;
}

Point::Latitude::Latitude(ValueType value) : value(value)
{
}

Point::Latitude Point::Latitude::operator-() const
{
    return Point::Latitude{-value};
}

Point::Latitude::operator ValueType() const
{
    return value;
}

Point::Longitude::Longitude(ValueType value) : value(value)
{
}

Point::Longitude Point::Longitude::operator-() const
{
    return Point::Longitude{-value};
}

Point::Longitude::operator ValueType() const
{
    return value;
}

Point::Latitude operator"" _lat(long double a)
{
    return Point::Latitude(a);
}

Point::Longitude operator"" _lon(long double a)
{
    return Point::Longitude(a);
}

std::string Point::to_string() const
{
    return "(" + std::to_string(longitude_) + ", " + std::to_string(latitude_) + ")";
}

}  // namespace Core::Common::Geometry
