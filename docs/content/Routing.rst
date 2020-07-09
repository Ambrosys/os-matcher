
.. _routing:

====================
OS-Matcher - Routing
====================

Routing and Matching seems sometimes interchangeable, but they are not.

Routing is defined as a threefold-function :math:`R(A,B,M) = r` where :math:`M` is the street map and :math:`A` and :math:`B`
are start and endpoint of the street map. :math:`R` returns a :term:`valid navigation route` from :math:`A` to :math:`B` on the street map :math:`M`.

The street map :math:`M` in use is the `OSM map <https://www.openstreetmap.org>`_.
Start and endpoint :math:`A` and :math:`B` are GPS locations, which not necessarily lay on the street map.

The routing algorithm falls apart into four parts:

* `Candidate Search`_
* `Routing`_
* `Clustering`_
* `Final Evaluation`_

.. _routing_candidate-search:

Candidate Search
----------------

Since the start and end points are potentially unbound GPS locations, the algorithm starts with a candidate search
(using :class:`class SamplingPointFinder <AppComponents::Common::Filter::SamplingPointFinder>` with a definable search radius)
to find nearby street segments from :math:`M`
resulting in a set of street segments for both points, :math:`S_A` and :math:`S_B`.

.. image:: images/Routing_01_candidates.png
   :name: Routing_01_candidates
   :width: 1000
   :class: with-shadow
   :alt: Start and Endpoint with candidates

The point is than projected onto each segment of the set resulting in a set of candidate points :math:`C^A` and :math:`C^B`
with the candidates :math:`c_i^A` and :math:`c_j^B`.

The candidates are ordered according to their distance.
However this might not be unique as you see in the following image.

.. image:: images/Routing_02_candidates.png
   :name: Routing_02_candidates
   :width: 600
   :class: with-shadow
   :alt: Example for ambiguous candidate

The track data may contain direction data as well, which is considered the next decision criteria
(using :func:`function headingDifference() <AppComponents::Common::Filter::Routing::SamplingPointFinder::headingDifference>`).
This would result in a distinct decision in our example.

.. image:: images/Routing_03_candidates.png
   :name: Routing_03_candidates
   :width: 600
   :class: with-shadow
   :alt: Candidate rank with distance and heading

If even that is not enough to determine a priority ordering of candidates,
there are three more optional categories to deal with that issue:

* **originId** of the street segment from the input street map data the candidate is laying on (for example the id a street segment has in the osm database)
* **originOffset + streetSegmentIndex** (because it might arise that a street segment is divided in sub street segments)
* **streetIndex** of the street segment as it was placed in the internal data structure
  (as a last decision criteria which is guaranteed to be unique, for the rare cases when the input street map data has duplicate IDs)

.. _routing_routing:

Routing
-------

After the candidates are found, we have a list of pairs :math:`P = \{ (s,t) | s \in C^A, t \in C^B \}`.

The street map :math:`M` is in its core a graph of nodes and edges (:math:`G(V,N)`),
the candidates are points on the edges and therefore not part of the graph.
To overcome that, the nodes of the edge a candidate is placed on, become the first selected nodes in the routing.

.. image:: images/Routing_04_candidates.png
   :name: Routing_04_candidates
   :width: 600
   :class: with-shadow
   :alt: Candidates on one way streets

Notice, if the used street map supports information on travel directions,
here already a filtering step is done by only considering one node of the edge aka the street segment.
By that already reducing computation time.

For each combination of the one or two possible first nodes of :math:`A` and :math:`B`
now a routing is done on the graph of the street map using a routing algorithm.
The default algorithm in *OS-Matcher* is Dijkstra's algorithm with the geographical length of a edge as its cost function.

So we end up with a list of routes, one for each pair :math:`(s,t)` and for each combination of
possible nodes (:math:`n_i^s` and :math:`n_j^t`) for the edges (:math:`e_s` and :math:`e_t`)
:math:`s` and :math:`t` are placed on.

Since the routes start from candidates of the same track points :math:`A` and :math:`B`
often the resulting nodes will be very similar. And therefore the next step is clustering the routes to groups
which are basically representing the same route.

.. image:: images/Routing_05_routes.png
   :name: Routing_05_routes
   :width: 800
   :class: with-shadow
   :alt: Several similar routes

However remember that this assumption is of course not always true:

.. image:: images/Routing_06_diff_routes.png
   :name: Routing_06_diff_routes
   :width: 800
   :class: with-shadow
   :alt: Differing routes for candidates of the same point.

.. _routing_clustering:

Clustering
----------

A cluster is a set of routes which are basically representing the same route, a meta route. All routes in a cluster are ranked by a comparison
:class:`class BestSimilarRouteComparator <AppComponents::Common::Filter::Routing::Comparators::BestSimilarRouteComparator>`,
which is comparing the candidates rank according to the system in `Candidate Search`_.
Any new route whcih shall be sorted into a cluster is compared to the highest ranked member of that cluster
(the *role model* if you like) using a similarity function.

The :func:`function isSimilar() <AppComponents::Common::Filter::Routing::Comparators::isSimilar>`
compares two routes :math:`r_0` and :math:`r_1` by several criteria and only when all of them are met
the route will be added to the cluster.

* **maxLenghtDifference**, the outermost two routes may differ in length, the default recommendation is to set this value 4 times the candidate search radius.
* **The source node of one is contained by the other**, :math:`n^s_{r_1} \in r_0` or :math:`n^s_{r_0} \in r_1`
* **The target node of one is contained by the other**, :math:`n^t_{r_1} \in r_0` or :math:`n^t_{r_0} \in r_1`
* **Source and target node are not visited twice**

Note that the second and third criteria does not need to be fulfilled by by both routes, but just by one.

.. image:: images/Routing_07_similarity_wide.png
   :name: Routing_07_similarity
   :width: 1600
   :class: with-shadow
   :alt: Similarity criteria

Clustering is a way to solve rather edgy scenarios which still arise to often to ignore them.
The best candidate tends to deliver the real streetsegemnt from which the data of the track point originates but cannot assure that,
on the other hand is the shortest route between two candidates not necessarily the correct one.

As an example scenario: In :ref:`Routing_08_clustering` the track point :math:`A` originates indeed from the roundabout.
But due to the offset (either by the data noise or the rundabout is just wider than the line segment implies)
the best candidate on a one way at the side. The best (shortest) route however starts at the worst candidate,
while the real route starts at the intermediate one.

Clustering is a way to overcome those and similar situations by filtering unlikely and nonsense routes
which are just considered because of the candidate search. And the candidate hierarchy within a cluster
assures that we get the most accurate starting point for the meta route represented by that cluster.

.. figure:: images/Routing_08_clustering.png
   :name: routing_08_clustering
   :width: 1000
   :class: with-shadow
   :alt: Clustering

Now we have set of clusters, each with a role model.

.. _routing_final_evaluation:

Final Evaluation
----------------

From each cluster the role model is chosen and all of those role models are compared using the
:class:`class BestRouteComparator <AppComponents::Common::Filter::Routing::Comparators::BestRouteComparator>`.
The Comparator has three criteria:

* **lenght**, which selects the shortest route
* **cost**, which selects the route with the lowest routing costs
* **number of points**, which selects the route with the least number of nodes``

The comparator comes in two flavors of criteria preference:

* **cheapest**, criteria order :math:`cost > length > number of points`
* **shortest**, criteria order :math:`length > cost > number of points`

The best route according to this comparison is then the result of our routing :math:`R(A,B,M) = r`.
