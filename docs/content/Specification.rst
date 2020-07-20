=============
Specification
=============

General
=======

.. glossary::
   :sorted:

   street map

   offset
      Refers to an identifier (in combination with the original street identifier) of a street segment.

      It can be necessary to split a street given by a data source into multiple street segments, e.g. because of junctions.
      We explain the idea of the offset by the following example.

      .. figure:: img/Specification_Offset_1.png
         :class: with-shadow
         :scale: 90
         :alt: Original street map

         Original street map

      There are junctions of street 3 with the street 1 and street 2 at node 3 respectively node 5. We split the street 3 at these two nodes and receive 3
      street segments corresponding to street 3.

      .. figure:: img/Specification_Offset_2.png
         :name: Specification_Offset_2
         :class: with-shadow
         :scale: 80
         :alt: Original street map

         Created street segments

      In the end we receive (street id, offset) tuples. Such a tuple defines the beginning of a street segment. While
      the end of the street segment can be reconstructed implicit by the node count.


