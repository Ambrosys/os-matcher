/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-FileContributor: Fabian Sandoval Saldias <fabianvss@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <AppComponents/Common/Matcher/SamplingPointFinder.h>

#include <Core/Common/Geometry/Helper.h>

#include <amblog/global.h>

#include <boost/iterator/function_output_iterator.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <cassert>
#include <unordered_map>

namespace {

using StreetIndexGeoindexGeometry = boost::geometry::model::box<Core::Common::Geometry::Point>;
using StreetIndexGeoindexValue = std::pair<StreetIndexGeoindexGeometry, std::pair<size_t, size_t>>;
using StreetIndexGeoindexAlgorithm = boost::geometry::index::quadratic<16>;
using StreetIndexGeoindex = boost::geometry::index::rtree<StreetIndexGeoindexValue, StreetIndexGeoindexAlgorithm>;

/**
 * Add street index and all segment indices to spatial index.
 */
void addStreetIndex(StreetIndexGeoindex & geoindex, size_t const index, Core::Common::Geometry::LineString const & lineString, double const searchRadius)
{
    for (size_t i = 0; i < lineString.size() - 1; ++i)
    {
        auto box = Core::Common::Geometry::buffer(
            boost::geometry::return_envelope<StreetIndexGeoindexGeometry>(Core::Common::Geometry::Segment{lineString[i], lineString[i + 1]}), searchRadius);

        geoindex.insert(StreetIndexGeoindexValue{box, {index, i}});
    }
}

/**
 * @return vector of { streetIndex, streetSegmentIndex }
 */
std::vector<std::pair<size_t, size_t>> getStreetIndices(StreetIndexGeoindex & geoindex, Core::Common::Geometry::Point point)
{
    std::vector<std::pair<size_t, size_t>> indices;
    auto inserter = [&](StreetIndexGeoindexValue const & value) { indices.push_back(value.second); };

    geoindex.query(boost::geometry::index::contains(point), boost::make_function_output_iterator(inserter));
    return indices;
}

/**
 *
 * @param trackHeading Heading of the track point.
 * @param segmentHeading Heading in forwards-direction of the segment. The reversed `segmentHeading` may be considered for the difference-calculation.
 * @param travelDirection If `TravelDirection::both`, the direction nearest to the `trackHeading` is considered. If `TravelDirection::backwards` the `segmentHeading` is considered reversed.
 * @return { headingDifference, trackTravelDirection } `trackTravelDirection` is `TravelDirection::both` if `travelDirection` is also `TravelDirection::both` and the heading difference is nearly 90 degrees.
 */
std::pair<double, AppComponents::Common::Types::Street::TravelDirection>
headingDifference(double const trackHeading, double segmentHeading, AppComponents::Common::Types::Street::TravelDirection const travelDirection)
{
    // TODO: make `67.5` below configurable
    using namespace Core::Common;
    using AppComponents::Common::Types::Street::TravelDirection;
    switch (travelDirection)
    {
        case TravelDirection::both:
        {
            double a = Geometry::absHeadingDiff(trackHeading, segmentHeading);
            double b = Geometry::absHeadingDiff(trackHeading, Geometry::reversedHeading(segmentHeading));
            if (a <= b)
                return {a, a <= 67.5 ? TravelDirection::forwards : TravelDirection::both};
            else
                return {b, b <= 67.5 ? TravelDirection::backwards : TravelDirection::both};
        }
        case TravelDirection::forwards: return {Geometry::absHeadingDiff(trackHeading, segmentHeading), TravelDirection::forwards};
        case TravelDirection::backwards: return {Geometry::absHeadingDiff(trackHeading, Geometry::reversedHeading(segmentHeading)), TravelDirection::backwards};
    }
    assert(false);
    return {};
}

}  // namespace

namespace AppComponents::Common::Matcher {

SamplingPointFinder::SamplingPointFinder(SelectionStrategy selectionStrategy, double const searchRadius, double const maxHeadingDifference)
  : Filter("SamplingPointFinder"), selectionStrategy_(selectionStrategy), searchRadius_(searchRadius), maxHeadingDifference_(maxHeadingDifference)
{
    setRequirements({"PointList", "SegmentList", "TravelDirectionList"});
    setOptionals({"HeadingList", "PartialHeadingList"});
    setFulfillments({"SamplingPointList"});
}

bool SamplingPointFinder::operator()(
    Types::Track::PointList const & pointList,
    Types::Track::HeadingList const & headingList,
    Types::Street::SegmentList const & segmentList,
    Types::Street::TravelDirectionList const & travelDirectionList,
    Types::Routing::SamplingPointList & samplingPointList)
{
    assert(pointList.size() == headingList.size() || headingList.empty());
    assert(segmentList.size() == travelDirectionList.size());

    StreetIndexGeoindex geoindex;
    for (size_t i = 0; i < segmentList.size(); ++i)
        addStreetIndex(geoindex, i, segmentList[i].geometry, searchRadius_);

    for (size_t trackIndex = 0; trackIndex < pointList.size(); ++trackIndex)
    {
        std::vector<std::pair<size_t, size_t>> streetIndices = getStreetIndices(geoindex, pointList[trackIndex]);
        if (streetIndices.empty())
            continue;

        // `samplingPoints` are ordered by using the following comparator.
        auto samplingPointCandidateComparator = [&](Types::Routing::SamplingPointCandidate const & a, Types::Routing::SamplingPointCandidate const & b)
        {
            if (a.streetSegmentDistance < b.streetSegmentDistance)
                return true;
            if (a.streetSegmentDistance == b.streetSegmentDistance)
            {
                if (a.streetSegmentHeadingDifference < b.streetSegmentHeadingDifference)
                    return true;
                if (a.streetSegmentHeadingDifference == b.streetSegmentHeadingDifference)
                {
                    if (segmentList[a.streetIndex].originId < segmentList[b.streetIndex].originId)
                        return true;
                    if (segmentList[a.streetIndex].originId == segmentList[b.streetIndex].originId)
                    {
                        if (segmentList[a.streetIndex].originOffset + a.streetSegmentIndex < segmentList[b.streetIndex].originOffset + b.streetSegmentIndex)
                            return true;
                        if (segmentList[a.streetIndex].originOffset + a.streetSegmentIndex == segmentList[b.streetIndex].originOffset + b.streetSegmentIndex)
                        {
                            // Note: originId may not be unique, so it is possible to get to this point.
                            return a.streetIndex < b.streetIndex;
                        }
                    }
                }
            }
            return false;
        };
        auto candidates = std::set<Types::Routing::SamplingPointCandidate, decltype(samplingPointCandidateComparator)>{samplingPointCandidateComparator};

        for (auto [streetIndex, streetSegmentIndex] : streetIndices)
        {
            auto const & segmentGeometry = segmentList[streetIndex].geometry;
            auto segment = Core::Common::Geometry::Segment{segmentGeometry[streetSegmentIndex], segmentGeometry[streetSegmentIndex + 1]};
            auto [streetSegmentDistance, streetSegmentProjectedPoint, streetSegmentProjectedPointNormLength] = Core::Common::Geometry::geoDistance(pointList[trackIndex], segment);

            if (streetSegmentDistance > searchRadius_)
                continue;

            double streetSegmentHeading = Core::Common::Geometry::heading(segment);

            double streetSegmentHeadingDifference;
            Types::Street::TravelDirection streetSegmentTravelDirection;
            if (!headingList.empty())
                std::tie(streetSegmentHeadingDifference, streetSegmentTravelDirection)
                    = headingDifference(headingList[trackIndex], streetSegmentHeading, travelDirectionList[streetIndex]);
            else
            {
                streetSegmentHeadingDifference = 0.0;
                streetSegmentTravelDirection = travelDirectionList[streetIndex];
            }

            if (streetSegmentHeadingDifference > maxHeadingDifference_)
                continue;

            auto candidate = Types::Routing::SamplingPointCandidate{};
            candidate.streetIndex = streetIndex;
            candidate.streetSegmentDistance = streetSegmentDistance;
            candidate.streetSegmentIndex = streetSegmentIndex;
            candidate.streetSegmentProjectedPoint = streetSegmentProjectedPoint;
            candidate.streetSegmentProjectedPointNormLength = streetSegmentProjectedPointNormLength;
            candidate.streetSegmentHeading = streetSegmentHeading;
            candidate.streetSegmentHeadingDifference = streetSegmentHeadingDifference;
            candidate.streetSegmentTravelDirection = streetSegmentTravelDirection;

            candidates.insert(candidate);
        }

        switch (selectionStrategy_)
        {
            case SelectionStrategy::all:
                // Add all candidates.
                if (!candidates.empty())
                    samplingPointList.push_back({trackIndex, {candidates.begin(), candidates.end()}});
                break;
            case SelectionStrategy::best:
                // Only add the best candidate.
                if (!candidates.empty())
                    samplingPointList.push_back({trackIndex, {*candidates.begin()}});
                break;
            case SelectionStrategy::singles:
                // Only add a candidate if it is the only one.
                if (candidates.size() == 1)
                    samplingPointList.push_back({trackIndex, {*candidates.begin()}});
                break;
        }
    }

    APP_LOG(noise) << samplingPointList.size() << " samplingPoints found";

    return true;
}

}  // namespace AppComponents::Common::Matcher
