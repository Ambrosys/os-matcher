.. _track_point_projection:

========
Overview
========

Let :math:`M` be the street map and :math:`A` and :math:`B` two track points.

The street map :math:`M` in use is the `OSM map <https://www.openstreetmap.org>`_.
Start and endpoint :math:`A` and :math:`B` are GPS locations, which not necessarily lay on the street map.

.. _routing_candidate-search:

Candidate search
================

Since the start and end points are potentially unbound GPS locations, the algorithm starts with a candidate search
(using :class:`class SamplingPointFinder <AppComponents::Common::Filter::SamplingPointFinder>` with a definable search radius)
to find nearby street segments from :math:`M`
resulting in a set of street segments for both points, :math:`S_A` and :math:`S_B`.

.. figure:: img/Routing_01_candidates.png
   :name: Routing_01_candidates
   :class: with-shadow
   :scale: 50
   :alt: Start and Endpoint with candidates

   Start and Endpoint with candidates

The point is than projected onto each segment of the set resulting in a set of candidate points :math:`C^A` and :math:`C^B`
with the candidates :math:`c_i^A` and :math:`c_j^B`.

The candidates are ordered according to their distance.
However this might not be unique as you see in the following image.

.. figure:: img/Routing_02_candidates.png
   :name: Routing_02_candidates
   :class: with-shadow
   :scale: 50
   :alt: Example for ambiguous candidates

   Example for ambiguous candidates

The track data may contain direction data as well, which is considered as the next decision criteria
(using :func:`function headingDifference() <AppComponents::Common::Filter::Routing::SamplingPointFinder::headingDifference>`).
This would result in a distinct decision in our example.

.. figure:: img/Routing_03_candidates.png
   :name: Routing_03_candidates
   :class: with-shadow
   :scale: 50
   :alt: Candidate rank with distance and heading

   Candidate rank with distance and heading

If even that is not enough to determine a priority ordering of candidates,
there are three more optional categories to deal with that issue:

* **originId** of the street segment from the input street map data the candidate is laying on (for example the ID a street segment has in the osm database)
* **originOffset + streetSegmentIndex** (because it might arise that a street segment is divided in sub street segments)
* **streetIndex** of the street segment as it was placed in the internal data structure
  (as a last decision criteria which is guaranteed to be unique, for the rare cases when the input street map data has duplicate IDs)
