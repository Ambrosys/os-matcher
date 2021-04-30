.. _filter_csvtrackwriter:

==============
CsvTrackWriter
==============

.. admonition:: Disclaimer: Not part of the open source repo

   The *CsvTrackWriter* was designed for a specific use case with a specific file layout and may therefore not usable for the general public,
   so it is not part of the open source material.
   Feel free to `contact us <mailto:info@ambrosys.de?subject=[OS-Matcher]\ Interest\ in\ the\ CsvTrackWriter\ filter>`_
   if you need support for a CSV writer for tracks.

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

A CSV stream in the following format:

- CSV columns and corresponding types:
   - **time**: date/time in the format "yyyy-MM-ddThh:mm:ss" [ISO-8601] (example: ``2020-01-01T12:00:00``)
   - **lat**: floating-point
   - **lon**: floating-point
   - **heading**: [degree] floating-point
   - **velocity**: [m/s] floating-point

- CSV properties:
   - **Separator**: comma (:code:`,`)
   - **Floating point precision**: 14
   - **Column headers in first row**: Yes

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
