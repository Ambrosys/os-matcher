/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <ambpipeline/Filter.h>

#include <istream>

namespace AppComponents::Common::Reader {

class CsvTrackReader : public ambpipeline::Filter
{
public:
    CsvTrackReader(std::istream & input);
    bool operator()(
        AppComponents::Common::Types::Track::TimeList &,
        AppComponents::Common::Types::Track::PointList &,
        AppComponents::Common::Types::Track::HeadingList &,
        AppComponents::Common::Types::Track::VelocityList &);

private:
    std::istream & input_;
};

}  // namespace AppComponents::Common::Reader
