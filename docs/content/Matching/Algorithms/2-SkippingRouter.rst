.. _skipping_router:

==============
SkippingRouter
==============

This router consecutively routes from each sampling point to its successor with the :ref:`underlying router <candidate_backtrack_router>`.

If, at some point, it is not able to find a :term:`valid navigation route`, the router starts to skip sampling points consecutively in a specific, distance-dependant order.
The underlying routers are requested to ignores these points.

If the distance to skip reaches some threshold before finding a route, the router returnes no route.

Example
=======

In this example, the underlying router failed to route beyond sampling point **4**, as shown below.

.. figure:: img/2-SkippingBacktrackRouter-A.png
   :class: with-shadow
   :scale: 50
   :alt: No route triggers skip (A)

   No route triggers skip (A)

The router now starts skipping sampling points consecutively from both directions until we are able to find a route.

To decide whether we should skip over a sampling point from behind or from ahead,
we calculate the linear distances that need to be skipped and choose the option where this is minimal.
This selection process is illustrated in :numref:`2-SkippingBacktrackRouter-B`.

.. note::
   The linear distance is always measured from the first two sampling points causing the router to start the skipping process.
   It is calculated using the `haversine formula <https://en.wikipedia.org/wiki/Haversine_formula>`_.

.. figure:: img/2-SkippingBacktrackRouter-B.png
   :name: 2-SkippingBacktrackRouter-B
   :class: with-shadow
   :scale: 50
   :alt: Skipping candidates (B)

   Skipping candidates (B)

In our example the router choosed to skip sampling point **4**.

Now the routing continues, but the underlying router still does not find a route to go on.
So the selection process is repeated, as shown in :numref:`2-SkippingBacktrackRouter-C`.

.. figure:: img/2-SkippingBacktrackRouter-C.png
   :name: 2-SkippingBacktrackRouter-C
   :class: with-shadow
   :scale: 50
   :alt: Allowed skip (C)

   Allowed skip (C)

Now sampling points **4** and **5** are requested to be skipped.

Again, the underlying router failes to find a route
and selection process is repeated, as shown in :numref:`2-SkippingBacktrackRouter-D`.

.. figure:: img/2-SkippingBacktrackRouter-D.png
   :name: 2-SkippingBacktrackRouter-D
   :class: with-shadow
   :scale: 50
   :alt: Disallowed skip (D)

   Disallowed skip (D)

Now sampling points **3** to **5** are requested to be skipped.

Finally, the routing proceeds, illustrated in :numref:`2-SkippingBacktrackRouter-E`.

.. figure:: img/2-SkippingBacktrackRouter-E.png
   :name: 2-SkippingBacktrackRouter-E
   :class: with-shadow
   :scale: 50
   :alt: Successful skip (E)

   Successful skip (E)

Sampling points **3**, **4** and **5** are ignored now. However, the route is still consecutive.
