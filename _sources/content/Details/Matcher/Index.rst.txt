.. include:: ../../substitutions.rst

=======
Matcher
=======

Here you can find the basic data flow:

.. figure:: ../../OperationalData/DataSelection/img/generated/DataFlow.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: application dataflow

   Application dataflow

Each processing step of the *Intermediate* and *Core* phase is implemented as a *filter*.
A filter is a functor which can read, process and write specific data.

.. _context_concept:

An application holds all relevant data structures,
which are to be passed to filters, in its *context*.

A filter may have obligatory preconditions (*requirements*)
and/or optional preconditions (*optionals*). These can either be other filters or *features*,
which must be satisfied before execution.
After an execution of a filter, it satisfies specific features (*fulfillments*).

The :class:`Router class <AppComponents::Common::Matcher::Router>` for example
has the obligatory preconditions ``SamplingPointList``, ``Graph``, ``GraphEdgeMap``, ``StreetIndexMap`` and satisfies (after successful execution)
the features ``RouteList`` and ``RoutingStatistic``.
This is reflected by the parameters of its ``operator()``,
where the preconditions are constant references
and the satisfiable variables are writable references:

.. code-block::
   :linenos:

   Router::Router( /* ... */ ) : Filter( "Router" ), /* ... */
   {
      setRequirements({"SamplingPointList", "Graph", "GraphEdgeMap", "StreetIndexMap"});
      setOptionals({});
      setFulfillments({"RouteList", "RoutingStatistic"});
   }

   bool Router::operator()(
      Types::Routing::SamplingPointList const & samplingPointList,
      Types::Graph::Graph const & graph,
      Types::Graph::GraphEdgeMap const & graphEdgeMap,
      Types::Graph::StreetIndexMap const & streetIndexMap,
      Types::Routing::RouteList & routeList,
      Types::Routing::RoutingStatistic & routingStatistic)
  {
       /* ... */
  }

Based on these definitions (*requirements*, *optionals* and *fulfillments*)
a multithreaded execution pipeline can be created automatically
by adding the filters to the *pipeline* (see below).

You may adapt the definition of OsmMapReader to your needs or write your own filter and apply the above concept analogously.
In this chapter we are providing descriptions for input, output and the available configuration parameters
of all implemented filters of the |os-matcher|.


**List of filters**

.. toctree::
   :maxdepth: 1

   CsvTrackReader
   JsonTrackReader
   GeoJsonMapReader
   OsmMapReader
   SamplingPointFinder
   GraphBuilder
   Router
   CsvRouteWriter
   CsvSubRouteWriter
   GeoJsonRouteWriter
   CsvTrackWriter
   GeoJsonTrackWriter
   GeoJsonMapWriter
   JsonRouteStatisticWriter
