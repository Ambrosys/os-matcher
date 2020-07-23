.. include:: ../../../makros.rst

==============
Data selection
==============

The |os-matcher| supports various input and output formats and can easily be extended to support your own formats.

The core algorithms work on well-defined data structures which are processed by various :term:`filters<filter>`.
The source data is either completely provided by the input filters (coordinate/track and street map readers)
or it can partially be reconstructed by intermediate filters.
The core filters are performing the sampling point candidate search, street graph construction and the actual matching and routing of the track on the street map.
The resulting data gets passed to registered output filters to save, for example, the calculated route.

Basic data flow (simplified filter pipeline):

.. figure:: img/dataflow.png
   :class: with-shadow
   :scale: 50
   :alt: Filter dataflow

   Filter dataflow

.. toctree::
   :titlesonly:

   DataStructures
   InputData
