.. _skipping_router:

===========
Skip router
===========

This router tries to find the :term:`farthest route <farthest navigation route>` with the underlying :ref:`candidate_backtrack_router`.
If beneficial, specific :term:`sampling points <sampling point>` are requested to be ignored by the underlying routers.

Each time the underlying router has found its farthest route and this route does not reach the sampling point **Goal**,
the probability is high that some of the last sampling points's track positions were recorded with an inaccurate GPS source or due to street map errors.
The router therefore starts to skip sampling points consecutively in a specific, distance-dependant order,
starting with the last sampling point of the farthest route or the following.
The underlying routers then try to route farther by ignoring these sampling points.

If the distance to skip reaches some threshold before finding a route, the router returns the farthest route without skipping sampling points.

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
it calculates the linear distances that need to be skipped and chooses the option where this is minimal.
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

Development
===========

This router is implemented in the :class:`SkippingRouter <AppComponents::Common::Filter::Routing::SkippingRouter>` and :class:`SkippingBacktrackRouter <AppComponents::Common::Filter::Routing::SkippingBacktrackRouter>` classes.
