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

#include <boost/geometry/index/rtree.hpp>
#include <boost/iterator/function_output_iterator.hpp>

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

        // TODO: Research why sometimes min/max are wrong, f.ex. line ((12.794089999999999, 52.816599999999994), (12.794090000000001, 52.813930000000006))
        //       would not lead to the envelope ((12.794089999999999, 52.813930000000006), (12.794090000000001, 52.816599999999994)).
        //       Instead it has the same coordinates as the line.
        //       The following two if blocks work around this bug.
        if (box.min_corner().lat() > box.max_corner().lat())
        {
            auto min = box.min_corner().lat();
            box.min_corner().setLat(box.max_corner().lat());
            box.max_corner().setLat(min);
        }
        if (box.min_corner().lon() > box.max_corner().lon())
        {
            auto min = box.min_corner().lon();
            box.min_corner().setLon(box.max_corner().lon());
            box.max_corner().setLon(min);
        }

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

SamplingPointFinder::SamplingPointFinder(
    SelectionStrategy selectionStrategy,
    double const searchRadius,
    double const maxHeadingDifference,
    Types::Track::PointList const & pointList,
    Types::Track::HeadingList const & headingList,
    Types::Street::SegmentList const & segmentList,
    Types::Street::TravelDirectionList const & travelDirectionList)
  : Filter("SamplingPointFinder"), selectionStrategy_(selectionStrategy), searchRadius_(searchRadius), maxHeadingDifference_(maxHeadingDifference), pointList_(pointList),
    headingList_(headingList), segmentList_(segmentList), travelDirectionList_(travelDirectionList)
{
    setRequirements({});
    setOptionals({});
    setFulfillments({"SamplingPointList"});
}

bool SamplingPointFinder::operator()(Types::Routing::SamplingPointList & samplingPointList)
{
    assert(pointList_.size() == headingList_.size() || headingList_.empty());
    assert(segmentList_.size() == travelDirectionList_.size());

    StreetIndexGeoindex geoindex;
    for (size_t i = 0; i < segmentList_.size(); ++i)
        addStreetIndex(geoindex, i, segmentList_[i].geometry, searchRadius_);

    for (size_t trackIndex = 0; trackIndex < pointList_.size(); ++trackIndex)
    {
        std::vector<std::pair<size_t, size_t>> streetIndices = getStreetIndices(geoindex, pointList_[trackIndex]);
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
                    if (segmentList_[a.streetIndex].originId < segmentList_[b.streetIndex].originId)
                        return true;
                    if (segmentList_[a.streetIndex].originId == segmentList_[b.streetIndex].originId)
                    {
                        if (segmentList_[a.streetIndex].originOffset + a.streetSegmentIndex < segmentList_[b.streetIndex].originOffset + b.streetSegmentIndex)
                            return true;
                        if (segmentList_[a.streetIndex].originOffset + a.streetSegmentIndex == segmentList_[b.streetIndex].originOffset + b.streetSegmentIndex)
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
            auto const & segmentGeometry = segmentList_[streetIndex].geometry;
            auto segment = Core::Common::Geometry::Segment{segmentGeometry[streetSegmentIndex], segmentGeometry[streetSegmentIndex + 1]};
            auto [streetSegmentDistance, streetSegmentProjectedPoint, streetSegmentProjectedPointNormLength] = Core::Common::Geometry::geoDistance(pointList_[trackIndex], segment);

            if (streetSegmentDistance > searchRadius_)
                continue;

            double streetSegmentHeading = Core::Common::Geometry::heading(segment);

            double streetSegmentHeadingDifference;
            Types::Street::TravelDirection streetSegmentTravelDirection;
            if (!headingList_.empty())
                std::tie(streetSegmentHeadingDifference, streetSegmentTravelDirection)
                    = headingDifference(headingList_[trackIndex], streetSegmentHeading, travelDirectionList_[streetIndex]);
            else
            {
                streetSegmentHeadingDifference = 0.0;
                streetSegmentTravelDirection = travelDirectionList_[streetIndex];
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
