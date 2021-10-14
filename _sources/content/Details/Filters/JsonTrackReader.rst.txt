.. _filter_jsontrackreader:

===============
JsonTrackReader
===============

This filter receives a track in form of a JSON stream and produces an output which can be further processed by other filters,
e.g. the routing filters.

Input
=====

A JSON stream which contains a list with JSON objects. The JSON objects are having the following format:

   - pos_time
      - text in the format "YYYY-MM-DDTHH:MM:SSZ"
   - latitude
      - decimal number
   - longitude
      - decimal number
   - course [degrees]
      - decimal number
      - describes the heading
      - if available
   - speed [km/h]
      - decimal number
      - describes the velocity
      - will be converted to [m/s]

- Example of the JSON stream:

  .. code-block:: json

     [
        {
           "pos_time": "2018-07-03T04:22:53Z",
           "latitude": 50.050303,
           "longitude": 8.6567,
           "course": 291,
           "speed": 16,
        },
        ...
     ]


Output
======

- :class:`TimeList <Common::Types::Track::TimeList>`
- :class:`PointList <Common::Types::Track::PointList>`
- :class:`PartialHeadingList <Common::Types::Track::HeadingList>`
- :class:`VelocityList <Common::Types::Track::VelocityList>`

Configuration
=============
- None
