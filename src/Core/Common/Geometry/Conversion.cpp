/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Common/Geometry/Conversion.h>

#include <Generic/String/Split.h>

#include <sstream>

namespace Core::Common::Geometry {

nlohmann::json toGeoJson(LineString const & lineString)
{
    nlohmann::json array = nlohmann::json::array();
    for (size_t i = 0; i < lineString.size(); ++i)
    {
        auto const & point = lineString[i];
        array.push_back(nlohmann::json::array({point.lon(), point.lat()}));
    }
    return nlohmann::json{{"type", "LineString"}, {"coordinates", array}};
}

nlohmann::json toGeoJson(Point const & point)
{
    return nlohmann::json{{"type", "Point"}, {"coordinates", nlohmann::json::array({point.lon(), point.lat()})}};
}

LineString toLineString(nlohmann::json const & geoJson)
{
    if (geoJson.at("type").get<std::string>() != "LineString")
        throw std::runtime_error("GeoJson type is not LineString");
    LineString lineString;
    for (auto const & coordinate : geoJson.at("coordinates"))
        lineString.push_back(Point{Point::Longitude{coordinate.at(0).get<double>()}, Point::Latitude{coordinate.at(1).get<double>()}});
    return lineString;
}

std::string toWkt(Point const & point)
{
    std::ostringstream oss;
    oss << std::setprecision(14);
    oss << "POINT(";
    oss << point.lon() << ' ' << point.lat();
    oss << ')';
    return oss.str();
}

std::string toWkt(std::vector<Point> const & points)
{
    std::ostringstream oss;
    oss << std::setprecision(14);
    oss << "LINESTRING(";
    for (size_t i = 0; i < points.size(); ++i)
    {
        auto const & point = points[i];
        oss << point.lon() << ' ' << point.lat();
        if (i < points.size() - 1)
            oss << ',';
    }
    oss << ')';
    return oss.str();
}

std::string toWkt(std::vector<std::vector<Point>> const & points)
{
    std::ostringstream oss;
    oss << std::setprecision(14);
    oss << "MULTILINESTRING(";
    for (size_t i = 0; i < points.size(); ++i)
    {
        oss << "(";
        for (size_t j = 0; j < points[i].size(); ++j)
        {
            auto const & point = points[i][j];
            oss << point.lon() << ' ' << point.lat();
            if (j < points[i].size() - 1)
                oss << ',';
        }
        oss << ')';
        if (i < points.size() - 1)
            oss << ',';
    }
    oss << ')';
    return oss.str();
}

LineString toLineString(std::string const & wkt)
{
    size_t coordinatesOffset = 0;
    if (wkt.substr(0, 11) == "LINESTRING(")
        coordinatesOffset = 11;
    else if (wkt.substr(0, 12) == "LINESTRING (")
        coordinatesOffset = 12;
    else
        throw std::domain_error("String is not a LINESTRING.");
    if (wkt.substr(wkt.size() - 1, 1) != ")")
        throw std::domain_error("String is not a LINESTRING.");
    std::vector<std::string> pointsString;
    Generic::String::split(wkt.substr(coordinatesOffset, wkt.size() - coordinatesOffset - 1), ',', std::back_inserter(pointsString));
    LineString points;
    Point point;
    double x;
    for (auto const & pointString : pointsString)
    {
        std::istringstream iss(pointString);
        iss >> x;
        point.setLon(x);
        iss >> x;
        point.setLat(x);
        points.push_back(point);
    }
    return points;
}

Point toPoint(std::string const & wkt)
{
    if (wkt.substr(0, 6) != "POINT(" || wkt.substr(wkt.size() - 1, 1) != ")")
        throw std::domain_error("String is not a POINT.");
    Point point;
    double x;
    std::istringstream iss(wkt.substr(11, wkt.size() - 11 - 1));
    iss >> x;
    point.setLat(x);
    iss >> x;
    point.setLon(x);
    return point;
}

}  // namespace Core::Common::Geometry
