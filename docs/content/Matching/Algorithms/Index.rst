.. include:: ../../../makros.rst
.. _algorithms:

==========
Algorithms
==========

The |os-matcher| uses a hierarchy of routers, each addressing a specific domain of routing targets at a different scale.

The innermost router is responsible for routing on the street graph while the outermost tries to find routes as continuously as possible with the help of its underlying routers.

.. note::
   As described in the :ref:`matching_overview`, we have a :term:`street map` :math:`M` and :term:`sampling points <sampling point>` :math:`S_1` to :math:`S_n` which are to be mapped onto :math:`M`.

   To prepare the algorithms, a *simple directed graph* :math:`G` gets built from the street map :math:`M`, while interconnecting them with each other.

In the following sections, we describe and illustrate all the routers, beginning with the innermost one.

.. toctree::
   :titlesonly:

   7-Dijkstra
   6-SimpleRouter
   5-CandidateRouter
   4-CandidateBacktrackRouter
   3-SkippingRouter
   2-SkippingBacktrackRouter
   1-Router
