================
Routing overview
================

The *OS-Matcher* uses a hierarchy of routers, each addressing a specific domain of routing targets at a different scale.

The innermost router is responsible for routing on the street graph while the outermost tries to find routes as continuously as possible with the help of its underlying routers.

In the following sections, we describe and illustrate all the routers, beginning with the outermost one.

.. toctree::
   :titlesonly:

   1-Router
   2-SkippingBacktrackRouter
   3-SkippingRouter
   4-CandidateBacktrackRouter
   5-CandidateRouter
   6-SimpleRouter
   7-Dijkstra
