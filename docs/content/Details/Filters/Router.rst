.. _filter_router:

======
Router
======

This filter creates the :term:`route` representation from the :term:`track`, :term:`street map` and its :term:`street graph` representation.
It further creates statistical data about the routing.
It is therefore the core filter os the |os-matcher|.

For further explanation of its functionality refer to the :ref:`algorithms` chapter.

Input
=====

- mandatory
   - :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`
   - :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`
   - :class:`Graph <AppComponents::Common::Types::Graph::Graph>`
   - :class:`GraphEdgeMap <AppComponents::Common::Types::Graph::GraphEdgeMap>`
   - :class:`StreetIndexMap <AppComponents::Common::Types::Graph::StreetIndexMap>`
- optional
   - :class:`TimeList <AppComponents::Common::Types::Track::TimeList>`
   - :class:`VelocityList <AppComponents::Common::Types::Track::VelocityList>`

Output
======

- :class:`RouteList <AppComponents::Common::Types::Routing::RouteList>`
- :class:`RoutingStatistic <AppComponents::Common::Types::Routing::RoutingStatistic>`

Configuration
=============

- routeRestrictions: :class:`RouteRestrictions <AppComponents::Common::Filter::Routing::RouteRestrictions>`, consists of three fields:
   - maxVelocityDifference: [m/s] floating-point; how much the velocity values in the track points and the calculated velocity from the time values in the track points and the route length are allowed to differ
   - allowSelfIntersection: boolean; flag to either include or filter routes that are self intersecting
   - maxAngularDeviation: [degree] floating-point; maximum heading difference allowed between a used candidate in the routing and its corresponding sampling point
- maxSamplingPointSkippingDistance: [<unit>] floating-point
   - ?
- maxCandidateBacktrackingDistance: [<unit>] floating-point
   - ?
- maxClusteredRoutesLengthDifference: [<unit>] floating-point
   - length difference that is allowed for a route to become part of a route cluster (see :ref:`routing_clustering`)
- routeClusterPreference: :class:`RouteClusterPreference <AppComponents::Common::Filter::Routing::RouteClusterPreference>`, one of [cheapest, shortest] (see :ref:`routing_clustering`)
   - cheapest: the route with the lowest costs becomes the cluster representing route
   - shortest: the route with the shortest path becomes the cluster representing route
