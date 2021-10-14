.. _filter_geojsontrackwriter:

==================
GeoJsonTrackWriter
==================

This filter writes the internal representation of the :term:`track`
to a stream in `GeoJson <https://tools.ietf.org/html/rfc7946>`_ format.

Input
=====

- mandatory:
   - :class:`TimeList <Common::Types::Track::TimeList>`
   - :class:`PointList <Common::Types::Track::PointList>`
   - :class:`VelocityList <Common::Types::Track::VelocityList>`
- optional (mutually exclusive - only one of the following may be given) :
   - :class:`HeadingList <Common::Types::Track::HeadingList>`
   - :class:`PartialHeadingList <Common::Types::Track::HeadingList>`

Output
======

A GeoJson stream with the following contents

- feature:
   - **Point** [longitude, latitude]
- properties per feature:
   - **time** [ISO-8601] (example: ``2020-01-01T12:00:00``)
   - **velocity** [m/s]
   - **heading**, if available [degrees]
- Example:
   .. code-block:: json

      {
         "type": "FeatureCollection",
         "features": [
            {
               "type": "Feature",
               "geometry": {
                  "type": "Point",
                  "coordinates": [52.52, 13.40]
               },
               "properties": {
                  "time": "2020-07-20T11:11:11",
                  "velocity": 50.01,
                  "heading": 54.5
               }
            },
            ...
         ]
      }

Configuration
=============

- None
