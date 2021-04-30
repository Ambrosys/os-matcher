.. _filter_samplingpointfinder:

===================
SamplingPointFinder
===================

This filter projects the :term:`track` onto the :term:`street map`
by creating a sampling point list for each track point. For more information we recommend reading the chapter :ref:`Track point projection<track_point_projection>`.

Input
=====

- mandatory
   - :class:`PointList <AppComponents::Common::Types::Track::PointList>`
   - :class:`SegmentList <AppComponents::Common::Types::Segment::SegmentList>`
   - :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`
- optional (mutually exclusive - only one of the following may be given) :
   - :class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`
   - :class:`PartialHeadingList <AppComponents::Common::Types::Track::HeadingList>`

Output
======

- :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`

Configuration
=============

- selectionStrategy: :class:`SelectionStrategy<AppComponents::Common::Filter::SelectionStrategy>`
   - one of the values in [all, best, single]
   - **all**: all candidates are added to the sampling point
   - **best**: only the best candidate is added to the sampling point (see :ref:`routing_candidate-search`)
   - **single**: the sampling point is only considered when exactly one candidate is found, which is added
- searchRadius: [m] floating-point
   - Value is implicitly used to determine the maximum distance a track point can have from a :term:`street segment`, it is however not used as a 'radius'.
- maxHeadingDifference: [degree] floating-point
   - allowed :term:`heading` difference between a track point and the corresponding candidate
     (the heading of a candidate depends on the street segment it is projected onto and the driving orientation)
   - only used if heading data is available
