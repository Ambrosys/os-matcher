.. _filter_csvtrackwriter:

==============
CsvTrackWriter
==============

.. admonition:: Disclaimer: Not part of the open source repo

   The CsvTrackWriter was designed for a specific use case with specific file layout and is therefore not usable in
   different environments and not part included in the open source material.
   If you are in dire need of a filter exporting to csv `contact us <mailto:info@ambrosys.de?subject="[OS-Matcher] Interest in the CsvTrackWriter filter">`_
   and we will be glad to give you access or design a solution for your needs.

This filter writes the internal representation of the :term:`track`
to a stream in `CSV <https://www.ietf.org/rfc/rfc4180.txt>`_ format.

Input
=====

- mandatory:
   - :class:`TimeList <AppComponents::Common::Types::Track::TimeList>`
   - :class:`PointList <AppComponents::Common::Types::Track::PointList>`
   - :class:`VelocityList <AppComponents::Common::Types::Track::VelocityList>`
- optional (mutually exclusive - only one of the following may be given) :
   - :class:`HeadingList <AppComponents::Common::Types::Track::HeadingList>`
   - :class:`PartialHeadingList <AppComponents::Common::Types::Track::HeadingList>`

Output
======

A CSV stream with the following properties

- CSV columns and corresponding types:
   - **time**: date/time in the format "yyyy-MM-ddThh:mm:ss" [ISO-8601] (example: ``2020-01-01T12:00:00``)
   - **lat**: floating-point
   - **lon**: floating-point
   - **heading**: [degree] floating-point
   - **velocity**: [m/s] floating-point

- CSV properties:
   - **Separator**: comma/:code:`,`
   - **Floating point precision**: 14
   - **Column headers in first row**: Yes

.. todo::

    The writer writes the column names in the first row,
    but the :ref:`filter_csvtrackreader` starts right away and would throw an exception, right?
    Make an Issue?

- Example Table

   .. code-block::

      time,lat,lon,heading,velocity
      "2020-01-01T12:00:00",52.52000,13.40500,315.0,12.5
      "2020-01-01T12:00:05",52.52001,13.40499,320.0,13.4
      ...
   representing:

   +-----------------------+----------+----------+---------+----------+
   | time                  | lat      | lon      | heading | velocity |
   +-----------------------+----------+----------+---------+----------+
   | "2020-01-01T12:00:00" | 52.52000 | 13.40500 | 315.0   | 12.5     |
   +-----------------------+----------+----------+---------+----------+
   | "2020-01-01T12:00:05" | 52.52001 | 13.40499 | 320.0   | 13.4     |
   +-----------------------+----------+----------+---------+----------+
   | ...                   | ...      | ...      | ...     | ...      |
   +-----------------------+----------+----------+---------+----------+

Configuration
=============

- None
