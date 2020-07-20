.. _filter_geojsonroutewriter:

==================
GeoJsonRouteWriter
==================

This filter writes the internal representation of the :term:`route`
to a stream in `GeoJson <https://tools.ietf.org/html/rfc7946>`_ format.

Input
=====

- :class:`RouteList <AppComponents::Common::Types::Routing::RouteList>`
- :class:`GraphEdgeMap <AppComponents::Common::Types::Graph::GraphEdgeMap>`
- :class:`NodeMap <AppComponents::Common::Types::Graph::NodeMap>`
- :class:`TimeList <AppComponents::Common::Types::Track::TimeList>`
- :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`
- :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`

Output
======

A GeoJson stream with the following content

- feature:
   - **LineString** [[longitude, latitude], [longitude, latitude], ... ]
- properties per feature:
   - **Id** of the street segment
   - **entry_time** [ISO-8601] (example: ``2020-01-01T12:00:00``)
   - **exit_time** [ISO-8601] (example: ``2020-01-01T12:00:00``)
- Example:
   .. code-block:: json

      {
         "type": "FeatureCollection",
         "features": [
            {
               "type": "Feature",
               "geometry": {
                  "type": "LineString",
                  "coordinates": [[52.52, 13.40], [52.405, 13.40], [52.30, 13.407], [52.562, 13.405]
               },
               "properties": {
                  "Id": "1234567",
                  "entry_time": "2020-07-20T11:11:11",
                  "exit_time": "2020-07-20T11:12:12"
               }
            },
            ...
         ]
      }


Configuration
=============

- None
