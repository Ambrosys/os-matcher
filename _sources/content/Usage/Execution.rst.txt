.. include:: ../substitutions.rst

=======================
Execute example matcher
=======================

Preparation
===========

To run the example application of the |os-matcher|, you need some data.

Database
--------

.. TODO: #53 new explanation for an example

You need a running OpenStreetMap database. We reference the server and the port as ``$DB_SERVER`` and ``$DB_PORT`` in the following.

Track
-----

You need a CSV file containing a track. The track file needs to have the following format:

.. code-block::

   2020-12-31T14:00:00.000;52.0000000000000;13.0000000000000;180.00;24.00

The fields are:

- date/time
- latitude
- longitude
- heading
- velocity

We reference the file as ``in/track.csv`` in the following.

Execution
=========

.. code-block::

   export LD_LIBRARY_PATH=build/lib
   build/bin/ExampleMatcher \
       --fsp-in in/track.csv \
       --map-out out/map.geojson \
       --route out/route.csv \
       --sub-route out/sub-route.csv \
       --route-geojson out/route.geojson \
       --route-statistic out/route-statistic.json \
       --pipeline out/pipeline.dot \
       --host $DB_SERVER \
       --port $DB_PORT \
       --log-level noise
