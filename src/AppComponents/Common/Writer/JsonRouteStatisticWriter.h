/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "IWriter.h"

#include <AppComponents/Common/Types/Routing/SamplingPoint.h>
#include <AppComponents/Common/Types/Routing/Statistic.h>
#include <AppComponents/Common/Types/Track/Time.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Writer {

class JsonRouteStatisticWriter : public ambpipeline::Filter, public IRouteStatisticWriter
{
public:
    JsonRouteStatisticWriter(std::ostream & output);

    bool operator()(
        std::ostream &,
        Types::Routing::RoutingStatistic const &,
        Types::Routing::SamplingPointList const &,
        Types::Track::TimeList const &
        );

    bool operator()(
        Types::Routing::RoutingStatistic const &,
        Types::Routing::SamplingPointList const &,
        Types::Track::TimeList const &
    );

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Writer
