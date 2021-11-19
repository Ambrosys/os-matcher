.. include:: ../../substitutions.rst

=======
Writer
=======

Here you can find the basic data flow:

.. figure:: ../../OperationalData/DataSelection/img/generated/DataFlow.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: application dataflow

   Application dataflow

Writers are the classes to fulfill the output phase,
by implementing one of the provided interfaces:

- :class:`IWriter class <AppComponents::Common::Writer::IWriter>`.
   - .. code-block::
      :linenos:

      struct IRouteWriter {

          /**
           * Writes the route to the output stream
           *
           * Needed for route geometry
           * - RouteList
           * - GraphEdgeMap
           * - SegmentList
           *
           * @return success status
           */
          virtual bool operator()(
              std::ostream &,
              Types::Routing::RouteList const &,
              Types::Graph::GraphEdgeMap const &,
              Types::Street::SegmentList const &,
              Types::Graph::NodeMap const &,
              Types::Track::TimeList const &,
              Types::Routing::SamplingPointList const &
              )=0;

      };
   - .. code-block::
      :linenos:

      struct ITrackWriter {

          /**
           * Writes track back from the four-fold representation of Tracks:
           *  - TimeList: List of timestamps
           *  - PointList: List of GeoPositions per timestamp
           *  - HeadingList: List of directions per timestamp
           *  - VelocityList: List of velocities per timestamp.
           *
           *  In general:
           *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
           *
           * @return success status
           */
          virtual bool operator()(
              std::ostream &,
              Types::Track::TimeList const &,
              Types::Track::PointList const &,
              Types::Track::HeadingList const &,
              Types::Track::VelocityList const &
              )=0;

      };

   - .. code-block::
      :linenos:

      struct IMapWriter {

          /**
           * Writes map back from the four-fold representation of streetmaps
           *  - SegmentList: List of street segments from the map
           *  - NodePairList: List of junction pairs, in which a segment lies in between
           *  - TravelDirectionList: List of allowed travel directions in orientation from the first node to the second node of the corresponding NodePair
           *  - HighwayList: List of street types for the corresponding street segment
           *
           *  In general:
           *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
           *
           * @return
           */
          virtual bool operator()(
              std::ostream &,
              Types::Street::SegmentList const &,
              Types::Street::NodePairList const &,
              Types::Street::TravelDirectionList const &,
              Types::Street::HighwayList const &
              )=0;

      };
   - .. code-block::
      :linenos:

      struct IRouteStatisticWriter {
          /**
           *
           */
           virtual bool operator() (
              std::ostream &,
              Types::Routing::RoutingStatistic const &,
              Types::Routing::SamplingPointList const &,
              Types::Track::TimeList const &
              ) = 0;
      };


An application holds all relevant data structures,
which are to be passed to writers, in its *context*.

**List of writers**

.. toctree::
   :maxdepth: 1

   CsvRouteWriter
   CsvSubRouteWriter
   GeoJsonRouteWriter
   CsvTrackWriter
   GeoJsonTrackWriter
   GeoJsonMapWriter
   JsonRouteStatisticWriter