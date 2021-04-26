#include <AppComponents/Common/Filter/Routing/DirectedCandidateRouter.h>
#include <AppComponents/Common/Filter/Routing/Helper.h>

namespace AppComponents::Common::Filter::Routing {

std::shared_ptr<Types::Routing::Route> DirectedCandidateRouter::operator()(SamplingPointsSelection const samplingPointsSelection) const
{
    auto const & sourceSamplingPoint = samplingPointList_[samplingPointsSelection.source.index];
    auto const & targetSamplingPoint = samplingPointList_[samplingPointsSelection.target.index];
    auto const & sourceCandidate = sourceSamplingPoint.candidates.at(samplingPointsSelection.source.candidate.index);
    auto const & targetCandidate = targetSamplingPoint.candidates.at(samplingPointsSelection.target.candidate.index);
    auto const & sourceGraphTriplePair = streetIndexMap_.at(sourceCandidate.streetIndex);
    auto const & targetGraphTriplePair = streetIndexMap_.at(targetCandidate.streetIndex);
    auto const & sourceGraphTriple = samplingPointsSelection.source.candidate.consideredForwards ? sourceGraphTriplePair.forwards.value() : sourceGraphTriplePair.backwards.value();
    auto const & targetGraphTriple = samplingPointsSelection.target.candidate.consideredForwards ? targetGraphTriplePair.forwards.value() : targetGraphTriplePair.backwards.value();

    auto const & sourceSegment = segmentList_.at(sourceCandidate.streetIndex);
    auto const & targetSegment = segmentList_.at(targetCandidate.streetIndex);

    auto const sourceEdge = std::get<1>(sourceGraphTriple);
    auto const targetEdge = std::get<1>(targetGraphTriple);

    auto const sourceNode = std::get<2>(sourceGraphTriple);
    auto const targetNode = std::get<0>(targetGraphTriple);

    /**
     * |      sourceEdge       |      routed edge      |      routed edge      |      targetEdge       |
     * +-----------------------+-----------------------+-----------------------+-----------------------+
     * O  x     x o   x     x  O  .     .     .     .  O  .     .     .     .  O  x     x   o x     x  O
     * +-----------------------+-----------------------+-----------------------+-----------------------+
     *            ^            ^                       ^                       ^            ^
     *            |            sourceNode                                      targetNode   |
     *            sourceCandidate.streetSegmentProjectedPoint                               targetCandidate.streetSegmentProjectedPoint
     *
     *           [o   x     x  |  .     .     .     .  |  .     .     .     .  |  x     x   o]
     *           generated routes
     */

    auto route = std::make_shared<Types::Routing::Route>(Types::Routing::Route{{sourceNode, samplingPointsSelection.source}, {targetNode, samplingPointsSelection.target}, {}});
    Core::Common::Geometry::LineString newSubRoute;

    auto addToNewSubRoute = [&](auto const & point)
    {
        // The following check is necessary for the first sub point and the end point, but to make the code simplier we call this function for every point.
        // This check accounts cases where the projected source and end points are on the exact same positions as existing points.
        if (newSubRoute.empty() || !(newSubRoute.back() == point))
            newSubRoute.push_back(point);
    };

    addToNewSubRoute(sourceCandidate.streetSegmentProjectedPoint);

    bool routedOnEdge = false;
    if (sourceEdge == targetEdge)
    {
        assert(sourceCandidate.streetIndex == targetCandidate.streetIndex);
        routedOnEdge = true;
        if (samplingPointsSelection.source.candidate.consideredForwards != samplingPointsSelection.target.candidate.consideredForwards)
            routedOnEdge = false;
        if (routedOnEdge)
        {
            if (samplingPointsSelection.source.candidate.consideredForwards)
            {
                if (sourceCandidate.streetSegmentIndex > targetCandidate.streetSegmentIndex)
                    routedOnEdge = false;
                else if (
                    sourceCandidate.streetSegmentIndex == targetCandidate.streetSegmentIndex
                    && sourceCandidate.streetSegmentProjectedPointNormLength > targetCandidate.streetSegmentProjectedPointNormLength)
                    routedOnEdge = false;
                if (routedOnEdge)
                    for (size_t i = sourceCandidate.streetSegmentIndex + 1; i <= targetCandidate.streetSegmentIndex; ++i)
                        addToNewSubRoute(sourceSegment.geometry.at(i));
            }
            else
            {
                if (sourceCandidate.streetSegmentIndex < targetCandidate.streetSegmentIndex)
                    routedOnEdge = false;
                else if (
                    sourceCandidate.streetSegmentIndex == targetCandidate.streetSegmentIndex
                    && sourceCandidate.streetSegmentProjectedPointNormLength < targetCandidate.streetSegmentProjectedPointNormLength)
                    routedOnEdge = false;
                if (routedOnEdge)
                    for (size_t i = sourceCandidate.streetSegmentIndex; i > targetCandidate.streetSegmentIndex; --i)
                        addToNewSubRoute(sourceSegment.geometry.at(i));
            }
        }
        if (routedOnEdge)
        {
            addToNewSubRoute(targetCandidate.streetSegmentProjectedPoint);
            route->subRoutes.emplace_back(Types::Routing::SubRoute{sourceEdge, 0.0, newSubRoute, geoDistance(newSubRoute)});
        }
    }

    if (!routedOnEdge)
    {
        if (samplingPointsSelection.source.candidate.consideredForwards)
            for (size_t i = sourceCandidate.streetSegmentIndex + 1; i < sourceSegment.geometry.size(); ++i)
                addToNewSubRoute(sourceSegment.geometry.at(i));
        else
            for (size_t i = sourceCandidate.streetSegmentIndex;; --i)
            {
                addToNewSubRoute(sourceSegment.geometry.at(i));
                if (i == 0)
                    break;
            }
        route->subRoutes.emplace_back(Types::Routing::SubRoute{sourceEdge, 0.0, newSubRoute, geoDistance(newSubRoute)});
        newSubRoute.clear();

        if (!(sourceNode == targetNode))
        {
            bool pathFound = false;
            auto path = algorithm_.run(sourceNode, targetNode);
            std::list<std::pair<Core::Graph::Edge, double>> path_reversed;
            for (auto const edge : path)
                path_reversed.emplace_front(edge.edge(), edge.cost());
            for (auto const edge : path_reversed)
            {
                auto streetEdge = graphEdgeMap_.at(edge.first);
                auto segment = segmentList_.at(streetEdge.streetIndex);
                if (streetEdge.forwards)
                    for (size_t i = 0; i < segment.geometry.size(); ++i)
                        addToNewSubRoute(segment.geometry.at(i));
                else
                    for (size_t i = segment.geometry.size(); i > 0; --i)
                        addToNewSubRoute(segment.geometry.at(i - 1));
                route->subRoutes.emplace_back(Types::Routing::SubRoute{edge.first, edge.second, newSubRoute, geoDistance(newSubRoute)});
                newSubRoute.clear();
                pathFound = true;
            }
            if (!pathFound)  // the router failed finding a route
            {
                route->subRoutes.clear();
                return route;
            }
        }

        if (samplingPointsSelection.target.candidate.consideredForwards)
            for (size_t i = 0; i <= targetCandidate.streetSegmentIndex; ++i)
                addToNewSubRoute(targetSegment.geometry.at(i));
        else
            for (size_t i = targetSegment.geometry.size(); i > targetCandidate.streetSegmentIndex + 1; --i)
                addToNewSubRoute(targetSegment.geometry.at(i - 1));
        addToNewSubRoute(targetCandidate.streetSegmentProjectedPoint);
        route->subRoutes.emplace_back(Types::Routing::SubRoute{targetEdge, 0.0, newSubRoute, geoDistance(newSubRoute)});
    }

    if (!configuration_.allowSelfIntersection && isSelfIntersectingRoute(*route))
    {
        route->subRoutes.clear();
        return route;
    }

    if (configuration_.maxAngularDeviation < 360.0 && !checkMaxAngularDeviation(*route, configuration_.maxAngularDeviation))
    {
        route->subRoutes.clear();
        return route;
    }

    if (!timeList_.empty() && !velocityList_.empty())
    {
        double routeLength = 0.0;
        std::optional<Core::Common::Geometry::Point> lastPoint;
        std::optional<Core::Common::Geometry::Point> secondLastPoint;
        for (auto const & subRoute : route->subRoutes)
        {
            routeLength += subRoute.length;

            if (configuration_.accountTurningCircleLength != 0.0)
            {
                // add turning circle length when routing back the same coordinates
                assert(!subRoute.route.empty());
                for (size_t pointIndex = subRoute.route[0] == lastPoint ? 1 : 0; pointIndex < subRoute.route.size(); pointIndex++)
                {
                    auto const point = subRoute.route[pointIndex];
                    if (secondLastPoint == point)
                        routeLength += configuration_.accountTurningCircleLength;
                    secondLastPoint = lastPoint;
                    lastPoint = point;
                }
            }
        }
        auto const dt = std::chrono::duration_cast<std::chrono::seconds>(timeList_.at(targetSamplingPoint.trackIndex) - timeList_.at(sourceSamplingPoint.trackIndex));
        auto const realVelocity = (velocityList_.at(targetSamplingPoint.trackIndex) + velocityList_.at(sourceSamplingPoint.trackIndex)) / 2.0;
        auto const calculatedVelocity = routeLength / dt.count();
        if (std::abs(calculatedVelocity - realVelocity) > configuration_.maxVelocityDifference)  // TODO: maybe consider street limits and car type
        {
            route->subRoutes.clear();
            return route;
        }
    }

    return route;
}

}  // namespace AppComponents::Common::Filter::Routing
