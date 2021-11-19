.. include:: ../../substitutions.rst

======
Reader
======

Here you can find the basic data flow:

.. figure:: ../../OperationalData/DataSelection/img/generated/DataFlow.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: application dataflow

   Application dataflow

Readers are the classes to fulfill the input phase,
by implementing one of the provided reader interfaces:

- :class:`ITrackReader class <AppComponents::Common::Reader::ITrackReader>`
   - .. code-block::
      :linenos:

      struct ITrackReader
      {
          virtual ~ITrackReader() {}

          /**
           * Fills the provided datastructures with the four-fold representation of Tracks:
           *  - TimeList: List of timestamps
           *  - PointList: List of GeoPositions per timestamp
           *  - HeadingList: List of directions per timestamp
           *  - VelocityList: List of velocities per timestamp
           *
           *  In general:
           *      len(TimeList) = len(PointList) = len(HeadingList) = len(VelocityList)
           *
           * @return success status
           */
          virtual bool operator()(
              Types::Track::TimeList &,
              Types::Track::PointList &,
              Types::Track::HeadingList &,
              Types::Track::VelocityList &
              ) = 0;
      }

- :class:`IMapReader class <AppComponents::Common::Reader::IMapReader>`
   - .. code-block::
      :linenos:

      struct IMapReader
      {
          virtual ~IMapReader() {}
          /**
           * Fills the provided datastructures with the four-fold representation of streetmaps
           *  - SegmentList: List of street segments from the map
           *  - NodePairList: List of junction pairs, in which a segment lies in between
           *  - TravelDirectionList: List of allowed travel directions in orientation from the first node to the second node of the corresponding NodePair
           *  - HighwayList: List of street types for the corresponding street segment
           *
           *  In general:
           *      len(SegmentList) = len(NodePairList) = len(TravelDirectionList) = len(HighwayList)
           *
           * @return success status
           */
          virtual bool operator()(
              Types::Street::SegmentList &,
              Types::Street::NodePairList &,
              Types::Street::TravelDirectionList &,
              Types::Street::HighwayList &) = 0;

          /**
           * Initialize IMapReader to only read map data along the PointList
           * @return
           */
          virtual bool init(
              Types::Track::PointList const &,
              double fetchCorridor,
              bool useSingleSearchCircle,
              std::optional<std::unordered_set<Types::Street::HighwayType>> const &
              ){ return false; };

      };

An application holds all relevant data structures,
which are to be passed to readers, in its *context*.

**List of readers**

.. toctree::
   :maxdepth: 1

   CsvTrackReader
   JsonTrackReader
   GeoJsonMapReader
   OsmMapReader
