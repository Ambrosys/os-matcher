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

You need a running OpenStreetMap database. We use ``$DB_HOST``, ``$DB_PORT``, ``$DB_NAME``, ``$DB_USER`` and ``$DB_PASS`` in the following.

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

Output
------

You need to create the directory ``out``.

Execution
=========

.. note::

   If you have run the example matcher and got the ``out/map.geojson`` map file,
   you can use this instead of the database for the next run.
   Note, that the map file only covers the surrounding area of the track.
   To use the map file, you need to:

   - Remove the arguments ``--host``, ``--port``, ``--db``, ``--dbuser`` and ``--dbpasswd``.
   - Change the argument ``--map-out`` to ``--map-in``.

Execution with Docker
---------------------

.. code-block::

   sudo docker run --rm -it \
       -v $PWD/in:/app/workspace/in:ro \
       -v $PWD/out:/app/workspace/out \
       --network host \
      ambrosys/os-matcher:latest \
       --track-in in/track.csv \
       --map-out out/map.geojson \
       --route out/route.csv \
       --sub-route out/sub-route.csv \
       --route-geojson out/route.geojson \
       --route-statistic out/route-statistic.json \
       --pipeline out/pipeline.dot \
       --host $DB_HOST --port $DB_PORT --db $DB_NAME --dbuser $DB_USER --dbpasswd $DB_PASS \
       --log-level noise

Execution with built binary
---------------------------

.. code-block::

   build/install/bin/AmbRouter \
       --track-in in/track.csv \
       --map-out out/map.geojson \
       --route out/route.csv \
       --sub-route out/sub-route.csv \
       --route-geojson out/route.geojson \
       --route-statistic out/route-statistic.json \
       --pipeline out/pipeline.dot \
       --host $DB_HOST --port $DB_PORT --db $DB_NAME --dbuser $DB_USER --dbpasswd $DB_PASS \
       --log-level noise
