/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "IWriter.h"

#include <AppComponents/Common/Types/Street/Highway.h>
#include <AppComponents/Common/Types/Street/NodePair.h>
#include <AppComponents/Common/Types/Street/Segment.h>
#include <AppComponents/Common/Types/Street/TravelDirection.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Writer {

class GeoJsonMapWriter : public IMapWriter
{
public:
    GeoJsonMapWriter(std::ostream & output);

    bool operator()(
        std::ostream &,
        Types::Street::SegmentList const &,
        Types::Street::NodePairList const &,
        Types::Street::TravelDirectionList const &,
        Types::Street::HighwayList const &
        );

    bool operator()(
        Types::Street::SegmentList const &,
        Types::Street::NodePairList const &,
        Types::Street::TravelDirectionList const &,
        Types::Street::HighwayList const &
    );

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Writer
