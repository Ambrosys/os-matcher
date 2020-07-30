========
Glossary
========

General
=======

.. glossary::
   :sorted:

   street map
       Refers to a data source with street map data or to the map itself as described by the data.
       A street map consist at least of a set of street segment geometries, connected by node points.

   track
      Refers to a data source with track data or to the track itself as described by the data.
      A track consists at least of an ordered list of geo points (track points).
      The track might consist of additional data like velocity, heading, timestamp etc.

   route
      A route usually is the result of a map matching process and is snapped to the geometry of the streets on a :term:`street map`.

   valid navigation route
      A valid navigation route is a consecutive :term:`route` which you could navigate.

      Currently the following constraints are considered:
         - Allowed travel direction
         - Existing street intersections
         - Self intersections (optional)
         - Velocity difference (optional; for street matching)
         - Angular deviation (optional; for street matching)

   farthest navigation route
      The farthest route is a :term:`valid navigation route` with its last :term:`sampling point` index as high as possible.

   sampling point
      A track point, which at least has one :term:`candidate<sampling point candidate>` on a :term:`street segment`.

   sampling point candidate
      A track point projected on a :term:`street segment`.
      The projection has to meet specific requirements in order to be considered a candidate.

      Candidates are ordered according to several criteria (see :ref:`routing_candidate-search`).

   street graph
      A *simple directed graph* representing the :term:`street map`.
      The nodes and edges of the street graph are tightly coupled to the street map data.
      Makes the street map applicable to routing algorithms.

   street segment
      A minimal street segment without intersections (nodes) inbetween. May consist of more than two point. The corresponding linestring of the :term:`street map` may have been cutted to satisfy this requirement.

Operational data details
========================

.. seealso::
   :ref:`type_listing`

.. glossary::
   :sorted:

   street index
      Shared index for :term:`street map` data (f.ex. for ``SegmentList``, ``NodePairList``, ``TravelDirectionList``).

   street segment index
      Index of a sub segment (consisting of two points) of a :term:`street segment`.

   track index
      Shared index for :term:`track` data (f.ex. for ``TimeList``, ``PointList``, ``HeadingList``).

   sampling point index
      Index for :term:`sampling points<sampling point>`.

   sampling point candidate index
      Index for :term:`sampling point candidates<sampling point candidate>`.
