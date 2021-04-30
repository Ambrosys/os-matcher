.. _dijkstra_router:

===============
Dijkstra router
===============

While building the route, it is essential to find the shortest path from one node on the :term:`street graph` (which corresponds to a node on the :term:`street map`) to another.

To find the path, the `Dijkstra's algorithm <https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm>`_ is used.

.. note::
   The *cost function* is set to the length of the geometry representing the edge in the :term:`street graph`, calculated with the `haversine formula <https://en.wikipedia.org/wiki/Haversine_formula>`_.

Example
=======

In the following example (see :numref:`6-DijkstraRouter-StreetGraph`), we want to find a path from node **Start** to node **Goal**.

.. figure:: img/generated/6-DijkstraRouter-StreetGraph.drawio.png
   :name: 6-DijkstraRouter-StreetGraph
   :alt: Street map and street graph
   :class: with-shadow
   :scale: 50

   Street map and street graph

After executing the Dijkstra's algorithm, we get the shortest route :math:`r` as illustrated in :numref:`6-DijkstraRouter-ShortestRoute`.

.. figure:: img/generated/6-DijkstraRouter-ShortestRoute.drawio.png
   :name: 6-DijkstraRouter-ShortestRoute
   :alt: Shortest route
   :class: with-shadow
   :scale: 50

   Shortest route

.. note::
   As noted in :ref:`matching`, the routes are in general very short.
   Between two :term:`sampling points <sampling point>`, many different routes may get calculated
   if the :term:`sampling point candidates <sampling point candidate>` were projected to different street segments.

   In the following chapters we learn how the small routes between the street map nodes are used to build the final route.
