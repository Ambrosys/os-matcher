.. _matching:

========
Matching
========

While :ref:`routing <routing>` is the solution to find a route in a graph for a given start and end point,
matching can be understood as the generalization of that approach towards a list of track points. In os-matcher this list consist of geo points and the graph is a street map.
In that way routing becomes applicable to the street matching problem which is common today especially in the tolling industry.

.. note::
   **Street Matching Problem**

   Given a time bound track and a street map we want to identify the most likely route over the map that was taken by the track provider.

.. seealso:: See :ref:`data_structures` for information of the track and map data.

The OS-Matcher is challenging this problem by solving iteratively only the next simple step of the matching.
And the algorithm is an extension of the steps described in :ref:`routing`.

First the track is processed to become a List of SamplingPoints. That means for each track point a :ref:`candidate-search` is performed,
identifying segments in the street map from which the data point might had originated.
Only those track points with at least one candidate are considered later on
since without a candidate on the street map the algorithm has no string o handle those track point.

.. note::
   Some implemented optimizations are dependent on additional meta data for the track like, heading and velocity. Not providing those can lead to poorly matchings.

.. figure:: images/Matching_01_sample_point.png
   :name: matching_01_sample_point
   :width: 1200
   :class: with-shadow
   :alt: Track to sample points

Beginning with the first Sampling Point a routing is performed with the successive sampling point.
After a successful routing we search from the end of our found routing to the next sampling point and perform the next routing.
Iteratively like that we get a matching of our whole track.

However an iterative matching approach like that can lead into situations where no routing can be performed even thou the real world track might would have been plausible.
Let us take our example from :ref:`matching_01_sample_point`_ and add an unfortunate one way in there that leads to a reconsideration of the track point :math:`E` into the sampling points with a candidate.
Now our iterative approach will find a route connecting the candidates of sampling point :math:`A` until :math:`E` but cannot get further.

.. figure:: images/Matching_02_dead_end.png
   :name: matching_02_dead_end
   :width: 1200
   :class: with-shadow
   :alt: Matching leads into dead end

The os-matcher idea is the usage of backtracking (see `backtracking <https://de.wikipedia.org/wiki/Backtracking>`_).

[Kobra, übernehmen Sie]

.. note::

   One of the major assumptions is that the os-matcher is only routing between two points which are relatively near to each other.
   Like that routing for each possible candidates pair as described in :ref:`routing` remains cheap since the routes are short considering only some street segments sometimes only one.
   However, if a track has many big holes (e.g. due to connection loss for an GPS vehicle track)
   this can lead to a sincere increase in computing time if the edge points just before and after the hole have several to many candidates
