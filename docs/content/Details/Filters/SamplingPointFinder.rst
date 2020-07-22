.. _filter_samplingpointfinder:

===================
SamplingPointFinder
===================

This filter projects the :term:`track` onto the :term:`street map`
by creating a sampling point list for each track point worth considering
as described in the chapter :ref:`Track point projection<track_point_projection>`.

Input
=====

- mandatory
   - :class:`PointList <AppComponents::Common::Types::Track::PointList>`
   - :class:`SegmentList <AppComponents::Common::Types::Segment::SegmentList>`
   - :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`
- optional (mutually exclusive - only one of the following may be given) :
   - :class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`
   - :class:`PartialHeadingList <AppComponents::Common::Types::Track::HeadingList>`

.. todo:: @bibermann
   In `SamplingPointFinder` gibt es die Zeile
   `assert( pointList.size() == headingList.size() || headingList.empty() );`
   -> das wiederspricht doch ganz klar der Idee von `PartialHeadingList` oder nicht?

Output
======

- :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`

Configuration
=============

- selectionStrategy: one of the values in [all, best, single]:class:`SelectionStrategy<AppComponents::Common::Filter::SelectionStrategy>`
   - **all**: all candidates are added to the sampling point
   - **best**: only the best candidate is added to the sampling point (see :ref:`routing_candidate-search`)
   - **single**: the sampling point is only considered when exactly one candidate is found, which is added
- searchRadius: [m] floating-point
   - Value is implicitly used to determine the maximum distance a track point can have from a :term:`street segment`, it is however not used as a 'radius'.
- maxHeadingDifference: [degree] floating-point (only if heading data is available)
   - Maximum :term:`heading` difference between the track point and a candidate to let the candidate be considered.
     (a candidate has a heading depending on the street segment it is projected onto and the driving orientation)

.. todo:: actually searchRadius is not a radius

   The method `buffer` uses the given distance to enlarge a box around a street segment,
   resulting in a greater distance than searchRadius than can lead to a candidate hit.
   I would argue that has to be corrected, either by changing the filter, or ba changing the naming of tha variable and docs accordingly.
   Make an issue?
