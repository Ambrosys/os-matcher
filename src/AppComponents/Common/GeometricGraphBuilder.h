#pragma once

#include <Core/Common/Geometry/Types.h>
#include <Core/Graph/Graph.h>

#include <boost/geometry/index/rtree.hpp>

namespace AppComponents::Common {

class GeometricGraphBuilder
{
public:
    explicit GeometricGraphBuilder(Core::Graph::Graph & graph, double maxDistanceInMeters = 0.1f);
    void operator()(Core::Common::Geometry::LineString const & lineString);

private:
    using Value = std::pair<Core::Common::Geometry::Point, Core::Graph::Node>;
    using Algorithm = boost::geometry::index::quadratic<16>;
    using GeoIndex = boost::geometry::index::rtree<Value, Algorithm>;

    Core::Graph::Node getOrCreateNode(Core::Common::Geometry::Point point);

    Core::Graph::Graph & graph_;
    double maxDistanceInMeters_;
    GeoIndex geoIndex_;
};

}  // namespace AppComponents::Common
