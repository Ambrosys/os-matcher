.. _filter_geojsonmapwriter:

================
GeoJsonMapWriter
================

This filter writes the internal representation of the :term:`street map`
to a stream in `GeoJson <https://tools.ietf.org/html/rfc7946>`_ format.

Input
=====

- :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`
- :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`
- :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`
- :class:`HighwayList <AppComponents::Common::Types::Street::HighwayList>`

Output
======

A GeoJson stream with the following contents

- feature:
   - **LineString** [[longitude, latitude], [longitude, latitude], ... ]
- properties per feature:
   - **Id**: Original ID of the street from which this segment originates
   - **Offset**: Node index at which this segment starts in the original street (see :term:`origin offset`)
   - **SourceNode**: Node identifier for the first point of the segment
   - **TargetNode**: Node identifier for the last point of the segment
   - **TravelDirection**: Allowed travel direction; one of [Both, Forwards, Backwards] (:class:`TravelDirection <AppComponents::Common::Types::Street::TravelDirection>`)
   - **Highway**: Highway type; one of [Motorway, Trunk, Primary, Secondary, Tertiary, MotorwayLink, TrunkLink, PrimaryLink, SecondaryLink, TertiaryLink, Unknown] (:class:`HighwayType <AppComponents::Common::Types::Street::HighwayType>`, based on `OSM-Highway type <https://wiki.openstreetmap.org/wiki/Key:highway>`_)
- Example:
   .. code-block:: json

      {
         "type": "FeatureCollection",
         "features": [
            {
               "type": "Feature",
               "geometry": {
                  "type": "LineString",
                  "coordinates": [[52.52, 13.40], [52.405, 13.40], [52.30, 13.407], [52.562, 13.405]]
               },
               "properties": {
                  "Id": 23315,
                  "Offset": 0,
                  "SourceNode": 4,
                  "TargetNode": 85,
                  "TravelDirection": "Backwards",
                  "Highway": "Primary"
               }
            },
            ...
         ]
      }


Configuration
=============

- None
