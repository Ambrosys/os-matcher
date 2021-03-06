/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Track/Time.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace Applications::ExampleMatcher::Filter {

class CsvSubRouteWriter : public ambpipeline::Filter
{
public:
    CsvSubRouteWriter(std::ostream & output);
    bool operator()(
        AppComponents::Common::Types::Routing::RouteList const &,
        AppComponents::Common::Types::Graph::GraphEdgeMap const &,
        AppComponents::Common::Types::Graph::NodeMap const &,
        AppComponents::Common::Types::Track::TimeList const &,
        AppComponents::Common::Types::Street::SegmentList const &,
        AppComponents::Common::Types::Routing::SamplingPointList const &);

private:
    std::ostream & output_;
};

}  // namespace Applications::ExampleMatcher::Filter
