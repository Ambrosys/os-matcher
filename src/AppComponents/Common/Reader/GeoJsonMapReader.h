/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>
#include <AppComponents/Common/Types/Track/Point.h>

#include <ambpipeline/Filter.h>

#include <istream>

namespace AppComponents::Common::Reader {

class GeoJsonMapReader : public ambpipeline::Filter
{
public:
    GeoJsonMapReader(std::istream & input);
    bool operator()(Types::Street::SegmentList &, Types::Street::NodePairList &, Types::Street::TravelDirectionList &, Types::Street::HighwayList &);

private:
    std::istream & input_;
};

}  // namespace AppComponents::Common::Reader
