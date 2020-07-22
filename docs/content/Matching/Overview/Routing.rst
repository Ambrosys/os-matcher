.. include:: ../../../content/substitutions.rst

.. _routing:

=======
Routing
=======

Routing is defined as a threefold-function :math:`R(A,B,M) = r` where :math:`M` is the street map and :math:`A` and :math:`B`
are start and endpoint of the street map. :math:`R` returns a :term:`valid navigation route` from :math:`A` to :math:`B` on the street map :math:`M`.

The street map :math:`M` in use is the `OSM map <https://www.openstreetmap.org>`_.
Start and endpoint :math:`A` and :math:`B` are GPS locations, which not necessarily lay on the street map.

The routing algorithm falls apart into three parts:

* :ref:`routing_routing`
* `Clustering`_
* `Final Evaluation`_

.. _routing_routing:

Routing
-------

After the candidates are found (see :ref:`routing_candidate-search`), we have a list of pairs :math:`P = \{ (s,t) | s \in C^A, t \in C^B \}`.

The street map :math:`M` is in its core a graph of nodes and edges (:math:`G(V,N)`),
the candidates are points on the edges and therefore not part of the graph.
To overcome that, the nodes of the edge a candidate is placed on, become the first selected nodes in the routing.

.. figure:: img/Routing_04_candidates.png
   :name: Routing_04_candidates
   :class: with-shadow
   :scale: 50
   :alt: Candidates on one way streets

   Candidates on one way streets

Notice, if the used street map supports information on travel directions,
here already a filtering step is done by only considering one node of the edge aka the street segment.

For each combination of the one or two possible first nodes of :math:`A` and :math:`B`
now a routing is done on the graph of the street map using a routing algorithm.
The default algorithm in |os-matcher| is Dijkstra's algorithm with the geographical length of an edge as its cost function.

So we end up with a list of routes, one for each pair :math:`(s,t)` and for each combination of
possible nodes (:math:`n_i^s` and :math:`n_j^t`) for the edges (:math:`e_s` and :math:`e_t`)
:math:`s` and :math:`t` are placed on.

Since all the routes are starting from candidates of the same track point :math:`A` and likewise are ending on candidates of the same track point :math:`B`,
often the resulting routes are very similar.

.. figure:: img/Routing_05_routes.png
   :name: Routing_05_routes
   :class: with-shadow
   :scale: 50
   :alt: Basically similar routes

   Basically similar routes

In :numref:`Routing_05_routes` we see two *basically similar* routes like that.

Contrary, in :numref:`Routing_06_diff_routes` we see two *basically different* routes.

.. figure:: img/Routing_06_diff_routes.png
   :name: Routing_06_diff_routes
   :class: with-shadow
   :scale: 50
   :alt: Basically different routes.

   Basically different routes.

When very similar and very different routes are likewise found for the same start and end points, as we see in the following example,
mathematically justifying how to choose the most realistic route is very difficult:

.. figure:: img/Routing_clustering_motivation.png
   :name: Routing_clustering_motivation
   :class: with-shadow
   :scale: 50
   :alt: Finding the most realistic route

   Finding the most realistic route

In :numref:`Routing_clustering_motivation`, :math:`C_1^A` is the best candidate and :math:`C_3^A` produces the shortest route, but both are not correct - :math:`C_2^A` leads to the actual route.

To solve the problem, the routes are getting clustered, where each cluster represents a set of *basically similar* routes.

.. _routing_clustering:

Clustering
----------

As motivated above, a cluster is a set of routes which are representing *basically similar* routes. All routes in a cluster are ranked by a comparison
:class:`class BestSimilarRouteComparator <AppComponents::Common::Filter::Routing::Comparators::BestSimilarRouteComparator>`,
which compares the candidate's rank according to the system described in :ref:`routing_candidate-search`.

Any new route which shall be placed into a cluster is compared to the highest ranked member of that cluster, the *role model* of that cluster, using a similarity function.
The :func:`function isSimilar() <AppComponents::Common::Filter::Routing::Comparators::isSimilar>`
compares two routes :math:`r_0` and :math:`r_1` against several criteria. Only when all of them are met,
the route will be added to the cluster.

The criteria are:

* **Max length difference**, the outermost two routes may differ in length (it is recommended to set this value to :math:`4 \cdot candidate\ search\ radius`).
* **The source node of one is contained by the other**, :math:`n^s_{r_1} \in r_0` or :math:`n^s_{r_0} \in r_1`
* **The target node of one is contained by the other**, :math:`n^t_{r_1} \in r_0` or :math:`n^t_{r_0} \in r_1`
* **Source and target node are not visited twice**

Note that the second and third criteria do not need to be fulfilled by both routes, only by one.

.. figure:: img/Routing_07_similarity_wide.png
   :name: Routing_07_similarity
   :class: with-shadow
   :scale: 50
   :alt: Similarity criteria

   Similarity criteria

Let's recap the example illustrated by :numref:`Routing_clustering_motivation`. The track point :math:`A` originates indeed from the roundabout.
But due to the drift of :math:`A` (may be induced due to data noise or inaccurate map data),
the best candidate sits on a one-way street to the south (:math:`C_1^A`). The best (shortest) route however starts at the most unlikely candidate (:math:`C_3^A`),
while the actual route starts at (:math:`C_2^A`).

Clustering is a way to overcome those and similar situations by filtering out unlikely routes
which are just considered because of its candidate rank.
Within a cluster, however, the candidate's rank assures that we get the most accurate starting point from all the *basically similar* routes.

.. figure:: img/Routing_08_clustering.png
   :name: routing_08_clustering
   :class: with-shadow
   :scale: 50
   :alt: Clustering

   Clustering

Now we have a set of clusters, each having a *role model*.

.. _routing_final_evaluation:

Final Evaluation
----------------

From each cluster the role model is chosen and all of those role models are compared using the
:class:`class BestRouteComparator <AppComponents::Common::Filter::Routing::Comparators::BestRouteComparator>`.
The Comparator has three criteria:

* **length**, which selects the shortest route
* **cost**, which selects the route with the lowest routing costs
* **number of points**, which selects the route with the least number of nodes

The comparator comes in two flavors of criteria preference:

* **cheapest**, criteria order :math:`cost > length > number\ of\ points`
* **shortest**, criteria order :math:`length > cost > number\ of\ points`

The best route according to this comparison is then the result of our routing :math:`R(A,B,M) = r`.
