/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "IWriter.h"

#include <AppComponents/Common/Types/Graph/EdgeMap.h>
#include <AppComponents/Common/Types/Routing/Edge.h>
#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Track/Time.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Writer {

class GeoJsonRouteWriter : public IRouteWriter
{
public:
    GeoJsonRouteWriter(std::ostream & output);

    bool operator()(
        std::ostream &,
        Types::Routing::RouteList const &,
        Types::Graph::GraphEdgeMap const &,
        Types::Street::SegmentList const &,
        Types::Graph::NodeMap const &,
        Types::Track::TimeList const &,
        Types::Routing::SamplingPointList const &);

    bool operator()(
        Types::Routing::RouteList const &,
        Types::Graph::GraphEdgeMap const &,
        Types::Graph::NodeMap const &,
        Types::Track::TimeList const &,
        Types::Street::SegmentList const &,
        Types::Routing::SamplingPointList const &);

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Writer
