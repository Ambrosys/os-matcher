.. include:: ../../substitutions.rst

.. _data_structures:

===============
Data structures
===============


The core processing component of the |os-matcher| is, of course, the router. To perform the routing it needs data which it gets from input :term:`filters<filter>` and intermediate filters.
The input filters may also depend on each other. An example: to be able to load the map from a database, the track is needed to define the boundaries.

Processed data
==============

Each processing component is designed as a filter with a well defined set of input data (requirements and optionals) as well as output data (fulfillments).

.. seealso:: See :ref:`filter` to learn how to implement and plug in a filter.

Example: Router
---------------

The router mandatorily needs data about the street topology and the :term:`track` point projections onto :ref:`street segments<street segment>`. For each track point, there might exist more than one :term:`candidate<sampling point candidate>` (see `routing`_).

It may optionally process the track point's timestamp and velocity, if present. In contrast, it does not need the track point's coordinates.

The output is the route along with additional data to help reason about the found route.

.. _type_listing:

Type listing
============

It follows an overview about all the data types that are passed along the filters.

For each track point:
   - coordinate pair, as :class:`PointList <AppComponents::Common::Types::Track::PointList>`
   - heading, as :class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`
   - timestamp, as :class:`TimeList <AppComponents::Common::Types::Track::TimeList>`
   - velocity, as :class:`VelocityList <AppComponents::Common::Types::Track::VelocityList>`
.. not used so far:
   - altitude, as :class:`AltitudeList <AppComponents::Common::Types::Track::AltitudeList>`

For each street segment:
   - coordinates, as :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`
   - junctions, as :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`
   - travel direction, as :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`
.. not used so far:
   - street kind, as :class:`HighwayList <AppComponents::Common::Types::Street::HighwayList>`

Street graph:
   - street graph, as :class:`Graph <AppComponents::Common::Types::Graph::Graph>`
   - mapping from graph edge to street segment, as :class:`GraphEdgeMap <AppComponents::Common::Types::Graph::GraphEdgeMap>`
   - mapping from graph node to street junction, as :class:`NodeMap <AppComponents::Common::Types::Graph::NodeMap>`
   - mapping from street segment to graph edge, as :class:`StreetIndexMap <AppComponents::Common::Types::Graph::StreetIndexMap>`

Routing:
   - resulting route data, as :class:`RouteList <AppComponents::Common::Types::Routing::RouteList>`
   - additional routing data per sampling point route data, as :class:`RoutingStatistic <AppComponents::Common::Types::Routing::RoutingStatistic>`
   - projected track points, as :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`
