.. _outermost_router:

================
Piecewise router
================

This router is responsible for finding consecutive :term:`routes as far as possible <farthest navigation route>` until reaching the :term:`sampling point` **Goal**.

Starting at the sampling point **Start**, it uses the underlying :ref:`skipping_router` to find the farthest consecutive route.
If the target sampling point could not been reached, it starts a new route and repeats the process, until the route is complete.

.. note::
   As this is the outermost router, gaps produced here will result in actual gaps in the final route.
   However, this will rarely happen, as the underlying routers are capable of resolving most of the routing problems that could arise.

Example
=======

In this example we try to find a route from the sampling point **1** to the sampling point **7**.

.. figure:: img/generated/1-PiecewiseRouter.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: Router functionality

   Router functionality

Starting at sampling point **1**, the underlying router finds its "longest" route no farther than to sampling point **3**.

A new route is then searched, starting at sampling point **4**.

.. note::
   Note that starting a new route from the next sampling point is different from skipping a sampling point.
   The latter actually results in a consecutive route, only that we would have ignored a sampling point (by routing around).
   In the case we have in our example, it is indeed not possible for the subsequent routers to route any further without having to skip too many sampling points and reaching some threshold.

As no consecutive route could be found here, the next route is then searched from sampling point **5** and finally reaches its goal at sampling point **7**.

Development
===========

This router is implemented in the :class:`Router class <AppComponents::Common::Filter::Router>`.
