.. _filter_geojsonmapreader:

================
GeoJsonMapReader
================

This filter receives a :term:`street map` in form of a GeoJSON stream and produces an output which can be further processed by other filters,
e.g. the routing filters.

Input
=====

- feature:
   - **LineString** [[longitude, latitude], [longitude, latitude], ... ]
- properties per feature:
   - **Id** of the street segment
   - **Offset** of the street segment **(was ist der offset?)**
   - **SourceNode** of the street segment
   - **TargetNode** of the street segment
   - **TravelDirection** [Both, Forwards or Backwards]
   - **Highway** [Motorway, Trunk, Primary, Secondary, Tertiary, MotorwayLink, TrunkLink, PrimaryLink, SecondaryLink, TertiaryLink or Unknown]
- Example:
   .. code-block:: json

       {
          "type": "FeatureCollection",
          "features": [
             {
                "type": "Feature",
                "geometry": {
                   "coordinates": [
                      [
                         12.6079441,
                         52.9935395
                      ],
                      [
                         12.6103468,
                         52.9915834
                      ],
                      [
                         12.6110675,
                         52.9909935
                      ]
                   ],
                   "type": "LineString"
                },
                "properties": {
                   "Id": 265483627,
                   "Offset": 0,
                   "SourceNode": 16,
                   "TargetNode": 9,
                   "TravelDirection": "Forwards",
                   "Highway": "Motorway"
                }
             },
             ...
          ]
       }

Output
======

- :class:`SegmentList <Types::Street::SegmentList>`
- :class:`NodePairList <Types::Street::NodePairList>`
- :class:`TravelDirectionList <Types::Street::TravelDirectionList>`
- :class:`HighwayList <Types::Street::HighwayList>`

Configuration
=============

- None
