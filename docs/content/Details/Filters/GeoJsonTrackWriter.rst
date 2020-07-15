.. _filter_geojsontrackwriter:

=========================
Filter GeoJsonTrackWriter
=========================

This filter writes the internal four fold representation of the input track
to a file in `GeoJson <https://tools.ietf.org/html/rfc7946>`_ format.

Input
=====

- mandatory:
   - :class:`TimeList <Common::Types::Track::TimeList>`
   - :class:`PointList <Common::Types::Track::PointList>`
   - :class:`VelocityList <Common::Types::Track::VelocityList>`
- optional (XOR - only one list of heading data can be passed) :
   - :class:`HeadingList <Common::Types::Track::HeadingList>`
   - :class:`PartialHeadingList <Common::Types::Track::HeadingList>`

Output
======

.. note::
   TODO: Decide which version of output is more nice or to keep both.

A GeoJson file with the following contents

- feature:
   - **Point**
- properties per feature:
   - **time***, in ISO-8601 format
   - **velocity**
   - **heading**, if part of the input data
- Example:
   .. code-block:: json

      {
         "type": "FeatureCollection",
         "features": [
            {
               "type": "Feature",
               "geometry": {
                  "type": "Point",
                  "coordinates": [<lon>, <lat>]
               },
               "properties": {
                  "time": <YYYY-mm-ddTHH:MM:SS>,
                  "velocity": <double>,
                  "heading": <double> # Only present if heading data is existent for this point
               }
            },
            ...
         ]
      }

Configuration
=============

- The filter is initialized with a `std::ostream <https://en.cppreference.com/w/cpp/io/basic_ostream>`_ object to define the file path the track is written to.