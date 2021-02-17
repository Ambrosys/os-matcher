.. _skipping_router:

===========
Skip router
===========

This router tries to find the :term:`farthest route <farthest navigation route>` with the underlying :ref:`candidate_backtrack_router`.
If beneficial, specific :term:`sampling points <sampling point>` are requested to be ignored by the underlying routers.

Each time the underlying router has found its farthest route and this route does not reach the sampling point **Goal**,
the probability is high that some of the last sampling points's track positions were recorded with an inaccurate GPS source or due to street map errors.
The router therefore starts to skip consecutive sampling points in a specific, skip-distance-dependant order,
starting with the last sampling point of the farthest route or the following.
The underlying routers then try to route farther by ignoring these sampling points.

If it is able to route farther, the skipping is considered successfull and it continues as usual.
It then may start a new skipping session, if necessary.

If the distance to skip reaches some threshold before finding a route,
the router returns the route before the last skipping session has begun.

Example
=======

In this example, the underlying router failed to route beyond sampling point **4**, as shown below.

.. figure:: img/generated/2-SkipRouter-A.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: Farthest route does not reach Goal

   Farthest route does not reach Goal

The router now starts skipping sampling points consecutively from both directions until it is able to find a route.

To decide whether it should skip over a sampling point from behind or from ahead,
it calculates the linear distances that need to be skipped (from sampling point to sampling point) and chooses the option where this is minimal.

.. note::
   Depending on the chosen strategy (:ref:`samplingPointSkipStrategy <router_filter_configuration>`),
   the distances are measured from the sampling points before/after the to-be-skipped sampling points (``includeEdgeCosts``)
   or from the outermost sampling points that are to be skipped (``excludeEdgeCosts``).
   In the latter case the costs are zero for the first trial; as a rule the sampling point ahead is choosen to be skipped first.

.. note::
   The selection process tries out three different consecutively growing sampling point selections in parallel
   and always chooses the next with the minimal distance to be skipped:

   1. The first growing selection consists of the sampling point `ahead and the following`.
   2. The second growing selection consists of the sampling point `before and the preceeding`.
   3. The third selection can grow in `both` directions, depending on which next sampling point to skip has the minimal distance to the one before.

For simplicity, we now focus on the strategy ``includeEdgeCosts`` and assume that only the third growing selection process is used (growing to `both` directions).

This selection process is illustrated in :numref:`2-SkipRouter-B`.

.. note::
   The distance is always measured as a linear distance,
   beginning at one of the first two sampling points causing the router to start the skipping process (in our example **4** and **5**).
   It is calculated using the `haversine formula <https://en.wikipedia.org/wiki/Haversine_formula>`_.

.. figure:: img/generated/2-SkipRouter-B.drawio.png
   :name: 2-SkipRouter-B
   :class: with-shadow
   :scale: 50
   :alt: Choosing sampling point to skip

   Choosing sampling point to skip

In our example the router choosed to skip sampling point **4**.

Now the routing continues, but the underlying router still does not find a route to go on.
So the selection process is repeated, as shown in :numref:`2-SkipRouter-C`.

.. figure:: img/generated/2-SkipRouter-C.drawio.png
   :name: 2-SkipRouter-C
   :class: with-shadow
   :scale: 50
   :alt: Skipped sampling point 4

   Skipped sampling point 4

Now sampling points **4** and **5** are requested to be skipped.

Again, the underlying router failes to find a route
and selection process is repeated, as shown in :numref:`2-SkipRouter-D`.

.. figure:: img/generated/2-SkipRouter-D.drawio.png
   :name: 2-SkipRouter-D
   :class: with-shadow
   :scale: 50
   :alt: Disallowed skip (D)

   Skipped sampling points 4 and 5

Now sampling points **3** to **5** are requested to be skipped.

Finally, the routing proceeds, illustrated in :numref:`2-SkipRouter-E`.

.. figure:: img/generated/2-SkipRouter-E.drawio.png
   :name: 2-SkipRouter-E
   :class: with-shadow
   :scale: 50
   :alt: Routed farther by skipping 3 sampling points

   Routed farther by skipping 3 sampling points

Sampling points **3**, **4** and **5** are ignored now. However, the route is still consecutive.
