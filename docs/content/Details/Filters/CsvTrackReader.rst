.. _filter_csvtrackreader:

==============
CsvTrackReader
==============

This filter receives a track in form of a CSV file and produces an output which can be further processed by other filters,
e.g. the routing filters.

Input
=====

* CSV file with the following content of a row: `time, latitude, longitude, heading, velocity`
* where the columns will need to have the following format:

   - time
      - date/time in the format "YYYY-MM-DDTHH:MM:SS"
   - latitude
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - longitude
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - heading
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - velocity
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)

Output
======

- :class:`TimeList <Common::Types::Track::TimeList>`
- :class:`PointList <Common::Types::Track::PointList>`
- :class:`HeadingList <Common::Types::Track::HeadingList>`
- :class:`VelocityList <Common::Types::Track::VelocityList>`

Configuration
=============

- None
