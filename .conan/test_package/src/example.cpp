
#include <OsMatcher.h>
#include <fstream>
#include <iostream>

namespace {

    using namespace osmatcher;


    struct TestStruct {

        bool testFunction(
            AppComponents::Common::Reader::CsvTrackReader &&,
            osmatcher::Reader::GeoJsonMapReader &&,
            osmatcher::Reader::JsonTrackReader &&,
            //osmatcher::Reader::OsmMapReader &&,

            osmatcher::Matcher::GraphBuilder &&,
            //osmatcher::Matcher::Router &&,
            osmatcher::Matcher::SamplingPointFinder &&,

            osmatcher::Writer::CsvRouteWriter &&,
            osmatcher::Writer::CsvSubRouteWriter &&,
            osmatcher::Writer::GeoJsonMapWriter &&,
            osmatcher::Writer::GeoJsonRouteWriter &&,
            osmatcher::Writer::GeoJsonTrackWriter &&,
            osmatcher::Writer::JsonRouteStatisticWriter &&
        ) {
            return true;
        };
    };
}

int main() {

    auto testStruct = TestStruct();

    std::basic_string<char> tmpname = strdup("/tmp/tmpfileXXXXXX");
    mkstemp(reinterpret_cast<char *>(&tmpname));
    auto input = std::ifstream{tmpname};
    auto output = std::ofstream{tmpname};

    auto reader = osmatcher::Reader::CsvTrackReader{input};
    testStruct.testFunction(
        osmatcher::Reader::CsvTrackReader{input},
        osmatcher::Reader::GeoJsonMapReader{input},
        osmatcher::Reader::JsonTrackReader{input},
        // TODO
        //osmatcher::Reader::OsmMapReader{putput},

        osmatcher::Matcher::GraphBuilder{},
        // TODO
        /*osmatcher::Matcher::Router{
            0.0, true, 0.0, 0.0,
            osmatcher::Matcher::Routing::SamplingPointSkipStrategy::includeEdgeCosts,
            0.0, 0.0,
            osmatcher::Matcher::Routing::RouteClusterPreference::cheapest
        },*/
        osmatcher::Matcher::SamplingPointFinder{
            osmatcher::Matcher::SamplingPointFinder::SelectionStrategy::all, 0.0, 0.0
        },

        osmatcher::Writer::CsvRouteWriter{output},
        osmatcher::Writer::CsvSubRouteWriter{output},
        osmatcher::Writer::GeoJsonMapWriter{output},
        osmatcher::Writer::GeoJsonRouteWriter{output},
        osmatcher::Writer::GeoJsonTrackWriter{output},
        osmatcher::Writer::JsonRouteStatisticWriter{output}
        );

    std::cout << "Reader/Matcher/Writer instantiated." << std::endl;
}
