=============
Specifications
=============

General
=======

.. glossary::
   :sorted:

   heading
   track point heading
      The heading of a track point is the nautical or geographical Azimuth, the clockwise in degrees measured angle between North and the heading direction.
      (North = heading of 0°; East = heading of 90°; South = heading of 180°; West = heading of 270°)

   street segment origin offset
      It can be necessary to split a street given by a data source into multiple street segments, e.g. because of junctions.
      We explain the idea of the offset by the following example.

      .. figure:: img/Specification_Offset_1.png
         :class: with-shadow
         :scale: 50
         :alt: Original street map

         Original street map

      By splitting the original street segment 3 at the junctions with the original street segments 1 and 2 we created three street segments corresponding to the original street segment 3, all starting at a different offset.

      .. figure:: img/Specification_Offset_2.png
         :name: Specification_Offset_2
         :class: with-shadow
         :scale: 50
         :alt: Original street map

         Created street segments

      In the end we have (street ID, offset) tuples. Such a tuple defines the beginning of a street segment. While
      the end of the street segment can be reconstructed implicit by adding the coordinate count of the street segment to the offset.

   street segment origin ID
      To a :term:`street segment` corresponding ID from the original data source.
