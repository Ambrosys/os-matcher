.. include:: ../../../content/substitutions.rst
.. _filter_router:

======
Router
======

This filter creates the :term:`route` representation from the :term:`track`, :term:`street map` and its :term:`street graph` representation.
It further creates statistical data about the routing.
It is therefore the core filter of the |os-matcher|.

For further explanation of its functionality, please refer to the :ref:`algorithms` chapter.

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

.. _router_filter_configuration:

Configuration
=============

- maxVelocityDifference: [m/s] ``double``
   Based on the time stamps in the :term:`track` data and the length of the route,
   it gets calculated how fast the vehicle must have been driven.
   This then gets compared with the mean velocity of the start and end points (ignoring the points inbetween).
   If the difference is greater than the one given, the route gets discarded.
- allowSelfIntersection: ``bool``
   - ``false``: Discards routes, whose start or end point occurs one more time within the route.
- maxAngularDeviation: [degree] ``double``
   - ``360``: Deactivates this restriction.
   - Other values: Discards routes which have a greater :term:`heading` difference between a used candidate and its corresponding sampling point.
     The algorithm to determine this is rather complex, see
     :func:`function checkMaxAngularDeviation <AppComponents::Common::Filter::Routing::checkMaxAngularDeviation>`
     for the implementation.
- accountTurningCircleLength: [m] ``double``
   - add this turning circle length to the route length when routing back the exact same coordinates (when the second-last coordinate equals the current coordinate)
- maxSamplingPointSkippingDistance: [m] ``double``
   - maximal allowed distance to jump forwards or backwards
- samplingPointSkipStrategy: :class:`enum SamplingPointSkipStrategy <AppComponents::Common::Filter::Routing::SamplingPointSkipStrategy>` (see :ref:`skipping_router`)
   - ``includeEdgeCosts``: The distances are measured from the sampling points before/after the to-be-skipped sampling points.
   - ``excludeEdgeCosts``: The distances are measured from the outermost sampling points that are to be skipped.
- maxCandidateBacktrackingDistance: [m] ``double``
   - maximal allowed distance to go back to search for other ways
- maxClusteredRoutesLengthDifference: [m] ``double``
   - length difference that is allowed for a route to become part of a route cluster (see :ref:`routing_clustering`)
   - should be 4 times :ref:`Sampling Point Finder’s<filter_samplingpointfinder>` searchRadius
- routeClusterPreference: :class:`enum RouteClusterPreference <AppComponents::Common::Filter::Routing::RouteClusterPreference>` (see :ref:`routing_clustering`)
   - ``cheapest``: Chooses from all best routes of the clusters the route with the lowest routing costs.
   - ``shortest``: Chooses from all best routes of the clusters the shortest route.
