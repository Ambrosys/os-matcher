.. _filter_osmmapreader:

============
OsmMapReader
============

This filter receives a :term:`street map` from OpenStreetMap by connecting to a PostGIS database and produces an output which can be further processed by other filters,
e.g. the routing filters.

Input
=====

- :class:`Connection <StreetMatcher::Common::Postgres::Connection>`
   - Provides the connection to the PostGIS database with the street map from OpenStreetMap.

- :class:`PointList <Types::Track::PointList>`
   - This PointList is an obligatory precondition for the OsmMapReader providing the track points. These track point are the basis to build a spatially limited street map. Otherwise the complete street map would be delivered.

Output
======

- :class:`SegmentList <Types::Street::SegmentList>`
- :class:`NodePairList <Types::Street::NodePairList>`
- :class:`TravelDirectionList <Types::Street::TravelDirectionList>`
- :class:`HighwayList <Types::Street::HighwayList>`

Configuration
=============

- highwaySelection: ``unordered_set<HighwayType>``
   Specifies a filter for the
   :enum:`highway type<AppComponents::Common::Types::Street::HighwayType::HighwayType>`.
- fetchCorridor: ``double``
   Specifies a buffer to extend the :term:`track` before searching.
   See `useSingleSearchCircle` below for details.
- useSingleSearchCircle: ``bool``
   - ``false``:
      Search within a tube around the :term:`track`. The search will be performed with the
      `PostGIS function ST_DWithin <https://postgis.net/docs/ST_DWithin.html>`_
      as follows: ``ST_DWithin( <track linestring>, planet_osm_line.way, <fetchCorridor> )``.

      .. comment::

          If the distance between two consecutive points is too long,
          this can result in gaps (missing :term:`street segments <street segment>`)
          (`Issue 66 <https://gl.ambrosys.de/os-matcher/os-matcher/issues/66>`_).
   - ``true``:
      Search within a big circle around the track.
      Its center is the midpoint of the line between the first and the last point of the :term:`track`,
      its radius is the (`distance of this two points`) / 2 + `fetchCorridor`.