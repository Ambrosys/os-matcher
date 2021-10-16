/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "IWriter.h"

#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Writer {

class GeoJsonTrackWriter : public ambpipeline::Filter, public ITrackWriter
{
public:
    GeoJsonTrackWriter(std::ostream & output);

    bool operator()(
        std::ostream &,
        Types::Track::TimeList const &,
        Types::Track::PointList const &,
        Types::Track::HeadingList const &,
        Types::Track::VelocityList const &
    );

    bool operator()(
        Types::Track::TimeList const &,
        Types::Track::PointList const &,
        Types::Track::HeadingList const &,
        Types::Track::VelocityList const &
        );

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Writer
