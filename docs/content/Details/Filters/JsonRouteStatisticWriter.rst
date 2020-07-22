.. _filter_jsonroutestatisticwriter:

========================
JsonRouteStatisticWriter
========================

Writes statistical data obtained from the :ref:`filter_router` and the internal :term:`track` presentation to a JSON stream.


Input
=====

- mandatory
   - :class:`RoutingStatistic <AppComponents::Common::Types::Routing::RoutingStatistic>`
   - :class:`SamplingPointList <AppComponents::Common::Types::Routing::SamplingPointList>`
- optional
   - :class:`TimeList <AppComponents::Common::Types::Routing::TimeList>`

Output
======

A JSON stream which contains a JSON object. The JSON object has the following format:

- **calculated**: Array of JSON objects with the following elements:
   - **source**: *SamplingPoint-JSON*
   - **target**: *SamplingPoint-JSON*
   - **cost**: decimal number, cost of the edge
   - **length**: decimal number, length of the geometry
   - **sub-routes-count**: integer number, amount of subroutes to fulfil the routing
- **visited**: Array of JSON objects with the following elements:
   - **successfull**: boolean value; true, if the routing was successful
   - **source**: *SamplingPoint-JSON*
   - **target**: *SamplingPoint-JSON*

.. todo:: typo in successfull -> successful

Def: SamplingPoint-JSON
-----------------------

- **SamplingPoint-JSON**: JSON object representing a sampling point with the following elements:
   - **index**: index of the representing :term:`track` point
   - **time**: [ISO-8601] time value of the corresponding :term:`track` point (example: ``2020-01-01T12:00:00``)
   - **candidate**: *SamplingPointCandidate-JSON*
   - **candidates-count**: integer number

Def: SamplingPointCandidate-JSON
--------------------------------

- **candidate**: JSON object representing the used candidate with the following elements:
   - **index**: index in the list of candidates of the corresponding sampling point
   - **considered-forwards**: [boolean], determines whether the candidate was, for this route, assumed to be orientated equally to the street map edge.
   - **distance**: [m], distance of the candidate to its sampling point


- Example of the JSON output stream:

   .. code-block:: json
      :emphasize-lines: 2,4,7,14,17,30,33,36,43,46

      {
         "calculated": [
            {
               "source": {
                  "index": 0,
                  "time": "2020-01-01T12:00:00",
                  "candidate": {
                     "index": 1,
                     "considered-forwards": true,
                     "distance": 2.73,
                  },
                  "candidates-count": 2,
               },
               "target": {
                  "index": 249,
                  "time": "2020-01-01T12:34:56",
                  "candidate": {
                     "index": 0,
                     "considered-forwards": false,
                     "distance": 0.98,
                  },
                  "candidates-count": 1,
               },
               "cost": 756.33,
               "length": 512.65,
               "sub-routes-count": 3,
            },
            ...
         ],
         "visited": [
            {
               "successfull": true,
               "source": {
                  "index": 0,
                  "time": "2020-01-01T12:00:00",
                  "candidate": {
                     "index": 1,
                     "considered-forwards": true,
                     "distance": 2.73,
                  },
                  "candidates-count": 2,
               },
               "target": {
                  "index": 249,
                  "time": "2020-01-01T12:34:56",
                  "candidate": {
                     "index": 0,
                     "considered-forwards": false,
                     "distance": 0.98,
                  },
                  "candidates-count": 1,
               }
            },
            ...
         ]
      }

Configuration
=============

- None
