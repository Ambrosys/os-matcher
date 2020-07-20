.. _filter_csvroutewriter:

==============
CsvRouteWriter
==============

This filter writes the internal representation of the :term:`route`
to a stream in `CSV <https://www.ietf.org/rfc/rfc4180.txt>`_ format.

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

A CSV stream with the following properties

- CSV header columns and corresponding types:
   - **osm_ids**: listing of the corresponding osm ids
   - **route**: LINESTRING [[longitude, latitude], [longitude, latitude], ... ]
   - **length** [m]: length of the route
   - **cost**: cost of the route
   - **sourceNode**: Node identifier for the first point of the segment
   - **targetNode**: Node identifier for the last point of the segment
   - **sourceSamplingPointTime**: date/time in the format "YYYY-MM-DDTHH:MM:SS"
   - **targetSamplingPointTime**: date/time in the format "YYYY-MM-DDTHH:MM:SS"
   - **sourceSamplingPointCandidateIndex**: todo
   - **targetSamplingPointCandidateIndex**: todo
   - **sourceSamplingPointCandidateConsideredForwards**: todo
   - **targetSamplingPointCandidateConsideredForwards**: todo
   - **routeStartPoint**: geo point of the startpoint, POINT [longitude, latitude]
   - **routeEndPoint**: geo point of the endpoint, POINT [longitude, latitude]
- CSV properties:
   - separator: semicolon
   - floating point precision: 14
- Example:
    .. code-block::

        osm_ids;route;length;cost;sourceNode;targetNode;sourceSamplingPointTime;targetSamplingPointTime;sourceSamplingPointCandidateIndex;targetSamplingPointCandidateIndex;sourceSamplingPointCandidateConsideredForwards;targetSamplingPointCandidateConsideredForwards;routeStartPoint;routeEndPoint
        23090008,317179950;LINESTRING(10.409835677053 53.271690899804,10.4098579 53.2716969,10.4103007 53.2718317,10.410396898775 53.271860113945);41.844870470798;0;2862;2862;2018-07-01T19:52:29;2018-07-01T19:52:32;0;0;1;1;POINT(10.409835677053 53.271690899804);POINT(10.410396898775 53.271860113945)
        ...

Configuration
=============

- None
