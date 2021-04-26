#pragma once

#include <AppComponents/Common/Types/Track/Heading.h>
#include <AppComponents/Common/Types/Track/Point.h>
#include <AppComponents/Common/Types/Track/Time.h>
#include <AppComponents/Common/Types/Track/Velocity.h>

#include <ambpipeline/Filter.h>

#include <ostream>

namespace AppComponents::Common::Filter {

class GeoJsonTrackWriter : public ambpipeline::Filter
{
public:
    GeoJsonTrackWriter(std::ostream & output);
    bool operator()(Types::Track::TimeList const &, Types::Track::PointList const &, Types::Track::HeadingList const &, Types::Track::VelocityList const &);

private:
    std::ostream & output_;
};

}  // namespace AppComponents::Common::Filter
