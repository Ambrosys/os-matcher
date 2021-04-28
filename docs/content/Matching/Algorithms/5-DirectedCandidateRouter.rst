.. include:: ../../substitutions.rst

.. _simple_router:

=========================
Directed candidate router
=========================

The |os-matcher| needs to find :term:`valid navigation routes <valid navigation route>`
between :term:`sampling point candidates <sampling point candidate>`, which is done by this router.

.. note::
   The sampling point candidates are mapped onto the street map geometry
   and have no corresponding nodes on the street map/graph (see :ref:`Track point projection <track_point_projection>`).

To be able to calculate a single route and to simplify this router,
direction information is required, additionally to the two sampling point candidates, as an input - the *sampling points selection*.

The router then tries to find the shortest route between these two points. Depending on the topology, it may use the underlying :ref:`dijkstra_router`.

Finally, the configuration options given in :ref:`routeRestrictions <router_filter_configuration>` are evaluated and may discard the found route.
Another route may then be choosen by the next-outer routers.

Examples
========

In the following examples, we want to find a route from sampling point candidate :math:`C_1^A` to :math:`C_1^B`.

Here, we focus on route construction; the :ref:`routeRestrictions <router_filter_configuration>` evaluation is not further described.

Single edge, simple case
------------------------

In the case that both candidates lay on the same street map/graph edge and :math:`C_1^A` is headed towards :math:`C_1^B` while both having the same orientation,
no street map/graph node needs to be traversed, as shown in :numref:`5-DirectedCandidateRouter-SingleEdge`.

.. figure:: img/generated/5-DirectedCandidateRouter-SingleEdge.drawio.png
   :name: 5-DirectedCandidateRouter-SingleEdge
   :alt: Single edge
   :class: with-shadow
   :scale: 50

   Single edge

The route can easily be constructed by following the geometry of the street map edge
and inserting the coordinates of :math:`C_1^A` and :math:`C_1^B` at the respective ends.

Other cases
-----------

If :math:`C_1^B` does not lay on the same edge as :math:`C_1^A` or the orientation does not match as in the previous example, this router has to seek to the next nodes first.

This is needed, because the next-inner router is only able to route from street map/graph nodes.

The following examples show the two possible outcomes.

Identical nodes
---------------

It may happen, that the nodes are identical, as illustrated by the blue-ish circle in :numref:`5-DirectedCandidateRouter-NeighboringEdge`.

.. figure:: img/generated/5-DirectedCandidateRouter-NeighboringEdge.drawio.png
   :name: 5-DirectedCandidateRouter-NeighboringEdge
   :alt: Neighboring edge
   :class: with-shadow
   :scale: 50

   Neighboring edge

No further routing is necessary to construct the route, similar to the first example.

Different nodes
---------------

In the last example, the nodes are not identical (blue-ish circles in :numref:`5-DirectedCandidateRouter-FarEdge`).

.. note::
   This case with different nodes would also apply in our first example, if at least one of the sampling point candidates were oriented differently.
   Note that this orientation is not derived from the heading of the track but rather an experiment of the next-outer router
   (all four possibilities may be tried, but in general the orientation of the first sampling point is fixed due to the preceding route).

.. figure:: img/generated/5-DirectedCandidateRouter-FarEdge.drawio.png
   :name: 5-DirectedCandidateRouter-FarEdge
   :alt: FarEdge
   :class: with-shadow
   :scale: 50

   Far edge

To complete the route from and to the blue-ish nodes, this scenario requires routing over street map/graph edges via the :ref:`dijkstra_router`.
