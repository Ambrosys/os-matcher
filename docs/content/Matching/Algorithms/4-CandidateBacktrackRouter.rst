========================
CandidateBacktrackRouter
========================

This router shall find the longest route beginning at a :term:`sampling point` **Start**.
Basically, it does so by successively routing from one sampling point to the next with its :ref:`underlying router <candidate_router>`.

But if it at any point is not able to find a :term:`valid navigation route`,
it is able to go back to previous sampling points in history (up to the configured maximum) and re-route from other candidates.
That means it tries out other routes that were not considered optimal in the first place.

It may happen that, even with this backtracking, the router is not able to route further a specific sampling point.
It then returns the longest route found so far.

Example
=======

In this example the driver takes a freeway exit ramp,
but the GPS tracker has poor signal and does not catch the turn fast enough.
As a consequence the best candidates are sitting next to the main freeway lane, as illustrated below.

.. figure:: img/4-CandidateBacktrackRouter-overview.png
   :class: with-shadow
   :scale: 50
   :alt: Freeway exit ramp example

   Freeway exit ramp example

As a consequence the routers will route on the main freeway lane and will not be able to continue at some point. Then backtracking starts.

.. figure:: img/4-CandidateBacktrackRouter-backtrack-1.png
   :class: with-shadow
   :scale: 50
   :alt: Needs backtracking

   Needs backtracking

While backtracking, already passed sampling points are considered again while choosing different candidates.
In the following three images we see the algorithm going back and trying out other candidates.

.. figure:: img/4-CandidateBacktrackRouter-backtrack-2.png
   :class: with-shadow
   :scale: 50
   :alt: Backtracking trial 1 failed

   Backtracking trial 1 failed

.. figure:: img/4-CandidateBacktrackRouter-backtrack-3.png
   :class: with-shadow
   :scale: 50
   :alt: Backtracking trial 2 failed

   Backtracking trial 2 failed

.. figure:: img/4-CandidateBacktrackRouter-backtrack-4.png
   :class: with-shadow
   :scale: 50
   :alt: Backtracking trial 3 failed

   Backtracking trial 3 failed

As you see in the next image, the router finally finds a valid navigation route
by choosing a different candidate for the southernmost sampling point.

.. figure:: img/4-CandidateBacktrackRouter-backtrack-5.png
   :class: with-shadow
   :scale: 50
   :alt: Backtracking successful

   Backtracking successful

In this simple example, the router has found a route by just trying out the next best candidate for the previous sampling points.

But other circumstances may be more complicated. It follows a list of things, this router has to consider:

* While backtracking, the router may go back beyond the **Start** sampling point.
* It may go beyond skipped sampling points (we will discuss this later in :ref:`skipping_router`).
* It may not go beyond the current route (which could be in the middle of the track, we will discuss this later in :ref:`outermost_router`).
* It may not go beyond the maximum backtracking distance (:ref:`maxCandidateBacktrackingDistance <router_filter_configuration>`).
* The farthest route (the one whose last sampling point has the highest index) may not be the last one the router has tried.
* The router needs to track already visited routes to be able to try different ones.
