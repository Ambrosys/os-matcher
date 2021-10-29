.. _filter_osmmapreader:

============
OsmMapReader
============

This filter receives a :term:`street map` from OpenStreetMap by connecting to a PostGIS database and produces an output which can be further processed by other filters,
e.g. the :ref:`routing filter <router_filter>`.

Input
=====

- :class:`Connection <StreetMatcher::Common::Postgres::Connection>`
   - Provides the connection to the PostGIS database with the street map from OpenStreetMap.

- :class:`PointList <AppComponents::Common::Types::Track::PointList>`
   - This PointList is an obligatory precondition for the OsmMapReader providing the track points. These track points are the basis to build a spatially limited street map to avoid the delivering of the complete street map.

Output
======

- :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`
- :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`
- :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`
- :class:`HighwayList <AppComponents::Common::Types::Street::HighwayList>`

Configuration
=============

- highwaySelection: ``unordered_set<HighwayType>``
   Specifies a filter for the
   :enum:`highway type<AppComponents::Common::Types::Street::HighwayType::HighwayType>`, based on `OpenStreetMap Key:highway <https://wiki.openstreetmap.org/wiki/Key:highway>`_.
- fetchCorridor: ``double``
   Specifies a buffer to extend the :term:`track` before searching.
   See `useSingleSearchCircle` below for details.
- useSingleSearchCircle: ``bool``
   - ``false``:
      Search within a tube around the :term:`track`. The search will be performed with the
      `PostGIS function ST_DWithin <https://postgis.net/docs/ST_DWithin.html>`_
      as follows: ``ST_DWithin( <track linestring>, planet_osm_line.way, <fetchCorridor> )``.
      If the distance between two consecutive points is too long,
      this can result in gaps (missing :term:`street segments <street segment>`).
   - ``true``:
      Search within a big circle around the track.
      Its center is the midpoint of the line between the first and the last point of the :term:`track`,
      its radius is the (`distance of this two points`) / 2 + `fetchCorridor`.
      If the distance between startpoint and endpoint is too small this can result in gaps (missing :term:`street segments <street segment>`).
      Imagine the route between these two points would proceed partially out ot the circle. Then some necessary parts of the map (out of the circle)
      would be missing and the matching cannot be complete.