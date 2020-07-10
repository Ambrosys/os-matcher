=======================
SkippingBacktrackRouter
=======================

This router consecutively routes from each sampling point to its successor with the underlying router.

If, at some point, it is not able to find a :term:`valid navigation route`, the router starts to skip sampling points consecutively in a specific, distance-dependant order.
The underlying routers are requested to ignores these points.


.. todo::

   The one or the other is correct:

   - If the distance to skip reaches some threshold before finding a route, the router cannot reach its goal and terminates.
   - If the distance to skip reaches some threshold before finding a route, the router stops and returnes the longest route found so far.

Example
=======

A
-

In this example, the underlying router failed to route from sampling point **4** to sampling point **5**.

.. image:: img/2-SkippingBacktrackRouter-A.png
   :alt: SkippingBacktrackRouter (A)

We now start skipping sampling points from both directions until we are able to find a route.

B
-

We start in choosing our first sampling point to skip and consecutively repeat the selection process if we keep failing in finding a route.

.. image:: img/2-SkippingBacktrackRouter-B.png
   :alt: SkippingBacktrackRouter (B)

To decide whether we should skip a sampling point behind or ahead of us, we calculate the distance from our first route that failed to the next sampling point we:

- would start from (when skipping a point behind)
- or would reach (when skipping a point ahead)

C
-

.. image:: img/2-SkippingBacktrackRouter-C.png
   :alt: SkippingBacktrackRouter (C)

D
-

.. image:: img/2-SkippingBacktrackRouter-D.png
   :alt: SkippingBacktrackRouter (D)

E
-

.. image:: img/2-SkippingBacktrackRouter-E.png
   :alt: SkippingBacktrackRouter (E)
