#pragma once

#include <Core/Common/Geometry/Types.h>

#include <nlohmann/json.hpp>

#include <string>

namespace Core::Common::Geometry {

nlohmann::json toGeoJson(LineString const & lineString);
nlohmann::json toGeoJson(Point const & point);

LineString toLineString(nlohmann::json const & geoJson);

std::string toWkt(Point const & point);
std::string toWkt(std::vector<Point> const & points);
std::string toWkt(std::vector<std::vector<Point>> const & points);
LineString toLineString(std::string const & wkt);
Point toPoint(std::string const & wkt);

}  // namespace Core::Common::Geometry
