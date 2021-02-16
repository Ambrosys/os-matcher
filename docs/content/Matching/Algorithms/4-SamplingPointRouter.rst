.. _candidate_router:

=====================
Sampling point router
=====================

This router is responsible for finding the best route between two given :term:`sampling points <sampling point>` :math:`Start` and :math:`Goal`.

It is composed of the following three operations.

1. Possible routes calculation
==============================

Because a sampling point may have more than one candidate and a driver can cross the projected point in two directions,
there are many possible options how to begin the routing between the two sampling points, as we see in :numref:`4-SamplingPointRouter-Motivation`.

.. figure:: img/generated/4-SamplingPointRouter-Motivation.drawio.png
   :name: 4-SamplingPointRouter-Motivation
   :class: with-shadow
   :scale: 50
   :alt: Motivation

   Motivation

The underlying :ref:`simple_router` needs a single *sampling points selection* which consists of the following data:

* The two :term:`sampling point candidates <sampling point candidate>` :math:`Start` and :math:`Goal`.
* Direction information for each sampling point candidate.

The set of possible *sampling points selections* is reduced by two givens (see also :ref:`routing_routing`):

* The initial direction of the :math:`Start` sampling point, induced by the preceding route (if it's not the first sampling point of a new route).
* The allowed travel directions.

.. comment::
   Already visited routes are discarded as well.
   TODO: Describe when this can happen, but finish the implementation first, because the logic may be subject to change.

Example
-------

In the following example, the current route ends at sampling point candidate :math:`C_2^{Start}`, that's why routes from other candidates (here :math:`C_1^{Start}`) are not considered:

.. figure:: img/generated/4-SamplingPointRouter-Overview.drawio.png
   :class: with-shadow
   :scale: 50
   :alt: Overview

   Overview

In :numref:`4-SamplingPointRouter-PossibleRoute1` and :numref:`4-SamplingPointRouter-PossibleRoute2` two possible routes have been calculated by the underlying router
for two possible directions of :math:`C_2^{Goal}`.

.. figure:: img/generated/4-SamplingPointRouter-PossibleRoute1.drawio.png
   :name: 4-SamplingPointRouter-PossibleRoute1
   :class: with-shadow
   :scale: 50
   :alt: Possible route 1

   Possible route 1

.. figure:: img/generated/4-SamplingPointRouter-PossibleRoute2.drawio.png
   :name: 4-SamplingPointRouter-PossibleRoute2
   :class: with-shadow
   :scale: 50
   :alt: Possible route 2

   Possible route 2

If :ref:`Router filter's <filter_router>` ``allowSelfIntersection`` is set to ``true``, also the following route would be possible:

.. figure:: img/generated/4-SamplingPointRouter-PossibleRoute3.drawio.png
   :name: 4-SamplingPointRouter-PossibleRoute3
   :class: with-shadow
   :scale: 50
   :alt: Possible route 3

   Possible route 3


2. Routes clustering
====================

All found routes are now clustered as described in :ref:`routing_clustering`.

3. Route selection
==================

The role model of all clusters are then sorted as described in :ref:`routing_final_evaluation` determining the final, most realistic route.

Development
===========

This router is implemented in the :class:`CandidateRouter class <AppComponents::Common::Filter::Routing::CandidateRouter>`.
