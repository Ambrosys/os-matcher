.. include:: ../../../content/substitutions.rst

=======
Filters
=======

Each processing step of an application is implemented as a *filter*.

Here you can find the basic data flow (simplified filter pipeline):

.. figure:: ../../OperationalData/DataSelection/img/generated/DataFlow.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: Filter dataflow

   Filter dataflow

A filter is a functor which can read, process and write specific data.

.. _context_concept:

An application holds all relevant data structures,
which are to be passed to filters, in its *context*.

A filter may have obligatory preconditions (*requirements*)
and/or optional preconditions (*optionals*). These can either be other filters or *features*,
which must be satisfied before execution.
After an execution of a filter, it satisfies specific features (*fulfillments*).

The :class:`OsmMapReader class <AppComponents::Common::Filter::OsmMapReader>` for example
has the obligatory precondition ``PointList`` and satisfies (after successful execution)
the features ``SegmentList``, ``NodePairList``, ``TravelDirectionList`` and ``HighwayList``.
This is reflected by the parameters of its ``operator()``,
where the preconditions are constant references
and the satisfiable variables are writable references:

.. code-block::
   :linenos:

   OsmMapReader::OsmMapReader( /* ... */ ) : Filter( "OsmMapReader" ), /* ... */
   {
       setRequirements( { "PointList" } );
       setOptionals(    {} );
       setFulfillments( { "SegmentList", "NodePairList", "TravelDirectionList", "HighwayList" } );
   }

   bool OsmMapReader::operator()(
       Types::Track::PointList const & pointList,
       Types::Street::SegmentList & segmentList,
       Types::Street::NodePairList & nodePairList,
       Types::Street::TravelDirectionList & travelDirectionList,
       Types::Street::HighwayList & highwayList )
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
