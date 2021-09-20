==========
Input data
==========

To be able to perform the routing, we need a minimum set of data. When the input filters are not able to provide all needed data intermediate filters could be used to reconstruct the missing entries if possible. Additional data can be provided to increase the accuracy.

Data overview
=============

Track
-----

To perform the routing or matching, the filters have to provide at least two coordinate pairs (latitude and longitude; :class:`PointList <AppComponents::Common::Types::Track::PointList>`).

If possible, the following data should be provided for each point, too:

- heading (does not have to be complete, points without a heading are allowed) (:class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`)
- timestamp (:class:`TimeList <AppComponents::Common::Types::Track::TimeList>`)
- velocity ( in m/s; :class:`VelocityList <AppComponents::Common::Types::Track::VelocityList>`)

Street Map
----------

To built the routable street graph, the filters have to provide the following data ***for each street item***:

- coordinates (at least two coordinate pairs) (:class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`)
- identifiers for the two endpoints (junctions) (:class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`)
- allowed travel direction (forwards, backwards, both) (:class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`)

Input filter
============

The data must be provided by one or more filters.

.. seealso:: See :ref:`filter` to learn how to implement and plug in a filter.

Track
-----

.. figure:: img/generated/Track.drawio.png
   :name: TrackFig
   :class: with-shadow
   :scale: 50
   :alt: Track data

   Track data

In :numref:`Fig. %s Track data <TrackFig>` you see an example track and the representing data lists which have to be filled by the filters.

For a track to be complete, the following requirements have to be fulfilled by one or more filters:

``PointList``
   - Data type: :class:`PointList <AppComponents::Common::Types::Track::PointList>`

``HeadingList`` or ``PartialHeadingList`` (both optional)
   - Use the latter if the listing may contain *NaN* items.
     Using ``PartialHeadingList``, a filter that can reconstruct the missing heading data (fulfilling ``HeadingList``) could easily be plugged in.
   - Data type: :class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`
   .. note::
      For *NaN* items, use:

      .. code-block::

         std::numeric_limits<AppComponents::Common::Types::Track::Heading>::quiet_NaN()

``TimeList`` (optional)
   - Data type: :class:`TimeList <AppComponents::Common::Types::Track::TimeList>`

``VelocityList`` (optional)
   - Data type: :class:`VelocityList <AppComponents::Common::Types::Track::VelocityList>`

Example skeleton:

.. code-block::
   :linenos:

   class MyTrackReader : public AppComponents::Common::Filter::Filter
   {
   public:
       MyTrackReader( std::istream & input );
       bool operator()(
           AppComponents::Common::Types::Track::TimeList &,
           AppComponents::Common::Types::Track::PointList &,
           AppComponents::Common::Types::Track::HeadingList &,
           AppComponents::Common::Types::Track::VelocityList & );

   private:
       std::istream & input_;
   };


   MyTrackReader::MyTrackReader( std::istream & input )
   : Filter( "MyTrackReader" ), input_( input )
   {
       setRequirements( {} );
       setOptionals( {} );
       setFulfillments( { "TimeList", "PointList", "HeadingList", "VelocityList" } );
   }

   bool MyTrackReader::operator()(
       Common::Types::Track::TimeList & timeList,
       Common::Types::Track::PointList & pointList,
       Common::Types::Track::HeadingList & headingList,
       Common::Types::Track::VelocityList & velocityList )
   {
       APP_LOG_TAG( noise, "I/O" ) << "Reading track";

       // read `input_` filling `timeList`, `pointList`, `headingList` and `velocityList`

       return true;
   }

Street Map
----------

.. figure:: img/generated/StreetMap.drawio.png
   :name: StreetMapFig
   :class: with-shadow
   :scale: 50
   :alt: Street map data

   Street map data

In :numref:`Fig. %s Street map data <StreetMapFig>` you see an example street map and the representing data lists which have to be filled by the filters.

For a street map to be complete, the following requirements have to be fulfilled by one or more filters:

``SegmentList``
   - Data type: :class:`SegmentList <AppComponents::Common::Types::Street::SegmentList>`

``NodePairList``
   - Data type: :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`

``TravelDirectionList``
   - Data type: :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`

Example skeleton:

.. code-block::
   :linenos:

   class MyStreetMapReader : public AppComponents::Common::Filter::Filter
   {
   public:
       MyStreetMapReader( std::istream & input );
       bool operator()(
           Types::Street::SegmentList &,
           Types::Street::NodePairList &,
           Types::Street::TravelDirectionList & );

   private:
       std::istream & input_;
   };


   MyStreetMapReader::MyStreetMapReader( std::istream & input )
   : Filter( "MyStreetMapReader" ), input_( input )
   {
       setRequirements( {} );
       setOptionals( {} );
       setFulfillments( { "SegmentList", "NodePairList", "TravelDirectionList" } );
   }

   bool MyStreetMapReader::operator()(
       Types::Street::SegmentList & segmentList,
       Types::Street::NodePairList & nodePairList,
       Types::Street::TravelDirectionList & travelDirectionList )
   {
       APP_LOG_TAG( noise, "I/O" ) << "Reading street map";

       // read `input_` filling `segmentList`, `nodePairList` and `travelDirectionList`

       return true;
   }
