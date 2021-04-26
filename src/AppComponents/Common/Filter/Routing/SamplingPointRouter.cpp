#include <AppComponents/Common/Filter/Routing/Helper.h>
#include <AppComponents/Common/Filter/Routing/SamplingPointRouter.h>

#include <Core/Common/Geometry/Helper.h>

#include <mutex>
#include <thread>
#include <unordered_set>

namespace AppComponents::Common::Filter::Routing {

namespace {

    using ClusteredRouteMatrix = std::vector<std::set<std::shared_ptr<Types::Routing::Route>, BestSimilarRouteComparator>>;
    using SamplingPointCandidateSelectionPair = std::pair<Types::Routing::SamplingPointCandidateSelection, Types::Routing::SamplingPointCandidateSelection>;

    std::vector<SamplingPointCandidateSelectionPair> selectCandidates(
        size_t const sourceSamplingPointIndex,
        size_t const targetSamplingPointIndex,
        Types::Routing::RouteList const & routeList,
        Types::Routing::SamplingPointList const & samplingPointList)
    {
        auto const & sourceSamplingPoint = samplingPointList[sourceSamplingPointIndex];
        auto const & targetSamplingPoint = samplingPointList[targetSamplingPointIndex];
        auto selections = std::vector<SamplingPointCandidateSelectionPair>{};
        size_t sourceCandidateTestBegin;
        size_t sourceCandidateTestEnd;
        std::optional<bool> sourceCandidateTestConsideredForwardsOptional;
        auto previousConnectedRoute = findPreviousConnectedRoute(sourceSamplingPointIndex, routeList);
        if (!previousConnectedRoute)
        {
            sourceCandidateTestBegin = 0;
            sourceCandidateTestEnd = sourceSamplingPoint.candidates.size();
        }
        else
        {
            sourceCandidateTestBegin = (*previousConnectedRoute)->target.samplingPoint.candidate.index;
            sourceCandidateTestEnd = sourceCandidateTestBegin + 1;
            sourceCandidateTestConsideredForwardsOptional = (*previousConnectedRoute)->target.samplingPoint.candidate.consideredForwards;
        }
        size_t targetCandidateTestBegin = 0;
        size_t targetCandidateTestEnd = targetSamplingPoint.candidates.size();
        for (size_t sourceCandidateIndex = sourceCandidateTestBegin; sourceCandidateIndex < sourceCandidateTestEnd; ++sourceCandidateIndex)
        {
            for (size_t targetCandidateIndex = targetCandidateTestBegin; targetCandidateIndex < targetCandidateTestEnd; ++targetCandidateIndex)
            {
                auto const & sourceCandidate = sourceSamplingPoint.candidates.at(sourceCandidateIndex);
                auto const & targetCandidate = targetSamplingPoint.candidates.at(targetCandidateIndex);

                std::vector<std::pair<bool, bool>> consideredForwardsPairs;
                if (not sourceCandidateTestConsideredForwardsOptional)
                {
                    bool sourceConsideredForwards = sourceCandidate.streetSegmentTravelDirection == Types::Street::TravelDirection::forwards;
                    bool targetConsideredForwards = targetCandidate.streetSegmentTravelDirection == Types::Street::TravelDirection::forwards;
                    if (sourceCandidate.streetSegmentTravelDirection != Types::Street::TravelDirection::both)
                    {
                        if (targetCandidate.streetSegmentTravelDirection != Types::Street::TravelDirection::both)
                        {
                            consideredForwardsPairs.emplace_back(sourceConsideredForwards, targetConsideredForwards);
                        }
                        else
                        {
                            consideredForwardsPairs.emplace_back(sourceConsideredForwards, true);
                            consideredForwardsPairs.emplace_back(sourceConsideredForwards, false);
                        }
                    }
                    else
                    {
                        if (targetCandidate.streetSegmentTravelDirection != Types::Street::TravelDirection::both)
                        {
                            consideredForwardsPairs.emplace_back(true, targetConsideredForwards);
                            consideredForwardsPairs.emplace_back(false, targetConsideredForwards);
                        }
                        else
                        {
                            consideredForwardsPairs.emplace_back(true, true);
                            consideredForwardsPairs.emplace_back(false, false);
                            consideredForwardsPairs.emplace_back(true, false);
                            consideredForwardsPairs.emplace_back(false, true);
                        }
                    }
                }
                else
                {
                    bool sourceConsideredForwards = *sourceCandidateTestConsideredForwardsOptional;

                    // Direction may have been choosen from previous sampling point, so check for validity and correct if necessary.
                    if (sourceConsideredForwards && sourceCandidate.streetSegmentTravelDirection == Types::Street::TravelDirection::backwards)
                        sourceConsideredForwards = false;
                    if (!sourceConsideredForwards && sourceCandidate.streetSegmentTravelDirection == Types::Street::TravelDirection::forwards)
                        sourceConsideredForwards = true;

                    bool targetConsideredForwards = targetCandidate.streetSegmentTravelDirection == Types::Street::TravelDirection::forwards;
                    if (targetCandidate.streetSegmentTravelDirection != Types::Street::TravelDirection::both)
                    {
                        consideredForwardsPairs.emplace_back(sourceConsideredForwards, targetConsideredForwards);
                    }
                    else
                    {
                        consideredForwardsPairs.emplace_back(sourceConsideredForwards, true);
                        consideredForwardsPairs.emplace_back(sourceConsideredForwards, false);
                    }
                }

                for (auto const & consideredForwardsPair : consideredForwardsPairs)
                    selections.emplace_back(
                        SamplingPointCandidateSelectionPair{{sourceCandidateIndex, consideredForwardsPair.first}, {targetCandidateIndex, consideredForwardsPair.second}});
            }
        }
        return selections;
    }

    std::shared_ptr<Types::Routing::Route> getBestRoute(std::shared_ptr<ClusteredRouteMatrix> const & clusteredRouteMatrix, RouteClusterPreference const routeClusterPreference)
    {
        if (clusteredRouteMatrix->empty())
            return nullptr;

        auto bestRoutes = std::set<std::shared_ptr<Types::Routing::Route>, BestRouteComparator>{routeClusterPreference};
        for (auto const & clusteredRoutes : *clusteredRouteMatrix)
            bestRoutes.insert(*clusteredRoutes.begin());

        return *bestRoutes.begin();
    }

    std::shared_ptr<Types::Routing::Route>
    routeCached(SamplingPointsSelection const samplingPointsSelection, DirectedCandidateRouter const & router, SamplingPointRouter::RouteMap & routeMap)
    {
        if (auto it = routeMap.find(samplingPointsSelection); it != routeMap.end())
            return it->second;
        else
        {
            auto route = router(samplingPointsSelection);
            return route;
        }
    }

    std::shared_ptr<ClusteredRouteMatrix>
    cluster(std::vector<std::shared_ptr<Types::Routing::Route>> const & routes, double const maxLengthDifference, Types::Graph::GraphEdgeMap const & graphEdgeMap)
    {
        auto clusteredRouteMatrix = std::make_shared<ClusteredRouteMatrix>();

        if (routes.empty())
        {
            return clusteredRouteMatrix;
        }

        for (auto const & route : routes)
        {
            bool foundCluster = false;
            for (auto & clusteredRoutes : *clusteredRouteMatrix)
            {
                auto const & firstRoute
                    = **clusteredRoutes.begin();  // If it is similar to one route, it should be also similar to all the others, so we only check against the first in the cluster.
                if (isSimilar(*route, firstRoute, maxLengthDifference, graphEdgeMap))
                {
                    clusteredRoutes.insert(route);
                    foundCluster = true;
                    break;
                }
            }
            if (!foundCluster)
            {
                clusteredRouteMatrix->emplace_back();
                clusteredRouteMatrix->back().insert(route);
            }
        }

        return clusteredRouteMatrix;
    }

}  // namespace

std::vector<std::shared_ptr<Types::Routing::Route>> SamplingPointRouter::calcPossibleRoutes(
    size_t const sourceSamplingPointIndex,
    size_t const targetSamplingPointIndex,
    VisitedRouteSet & visitedRouteSet,
    RouteMap & routeMap,
    Types::Routing::RoutingStatistic & routingStatistic,
    Types::Routing::RouteList const & routeList) const
{
    auto routes = std::vector<std::shared_ptr<Types::Routing::Route>>{};

    auto selections = selectCandidates(sourceSamplingPointIndex, targetSamplingPointIndex, routeList, samplingPointList_);

    // TODO: comment the comments below in to use multi-threading; ATTENTION: DirectedCandidateRouter or LemonDigraph or Dijkstra are not yet reentrant, this has to be fixed before
    //std::mutex mutex;
    //std::vector<std::thread> threads;
    for (auto const & selection : selections)
    {
        auto samplingPointsSelection = SamplingPointsSelection{{sourceSamplingPointIndex, selection.first}, {targetSamplingPointIndex, selection.second}};
        if (visitedRouteSet.find(samplingPointsSelection) != visitedRouteSet.end())
            continue;
        //threads.emplace_back(
        //    [&]() {
        auto const route = routeCached(samplingPointsSelection, router_, routeMap);
        //std::lock_guard<std::mutex> lock( mutex );
        if (not route->subRoutes.empty())
            routes.push_back(route);
        routeMap.insert({samplingPointsSelection, route});
        routingStatistic.calculated.insert({samplingPointsSelection, {route->cost(), route->length(), route->subRoutes.size()}});
        //    } );
    }
    //for( auto & thread : threads )
    //    thread.join();

    return routes;
}

std::shared_ptr<Types::Routing::Route> SamplingPointRouter::operator()(
    size_t const sourceSamplingPointIndex,
    size_t const targetSamplingPointIndex,
    Types::Routing::RouteList const & routeList,
    VisitedRouteSet & visitedRouteSet,
    RouteMap & routeMap,
    Types::Routing::RoutingStatistic & routingStatistic) const
{
    auto routes = calcPossibleRoutes(sourceSamplingPointIndex, targetSamplingPointIndex, visitedRouteSet, routeMap, routingStatistic, routeList);

    auto clusteredRouteMatrix = cluster(routes, configuration_.maxClusteredRoutesLengthDifference, graphEdgeMap_);

    auto route = getBestRoute(clusteredRouteMatrix, configuration_.routeClusterPreference);

    if (!route)
    {
        routingStatistic.visited.emplace_back(SamplingPointsSelection{{sourceSamplingPointIndex, {0, true}}, {targetSamplingPointIndex, {0, true}}}, false);
        return nullptr;
    }

    auto samplingPointsSelection = SamplingPointsSelection{route->source.samplingPoint, route->target.samplingPoint};
    visitedRouteSet.insert(samplingPointsSelection);
    routingStatistic.visited.emplace_back(samplingPointsSelection, true);

    return route;
}

}  // namespace AppComponents::Common::Filter::Routing
