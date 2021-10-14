.. _filter_csvtrackreader:

==============
CsvTrackReader
==============

This filter receives a track in form of a CSV stream and produces an output which can be further processed by other filters,
e.g. the routing filters.

Input
=====

* CSV stream with the following content of a row: `time, latitude, longitude, heading, velocity`
* where the columns will need to have the following format:

   - time
      - date/time in the format "yyyy-MM-ddThh:mm:ss" [ISO-8601] (example: ``2020-01-01T12:00:00``)
   - latitude
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - longitude
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - heading
      - floating-point (interpretable by `std::stod <https://en.cppreference.com/w/cpp/string/basic_string/stof>`_)
   - velocity [m/s]
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
