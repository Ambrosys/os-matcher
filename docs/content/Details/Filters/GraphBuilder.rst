.. _filter_graphbuilder:

============
GraphBuilder
============

This filter takes part of the intrnal representation of a :term:`street map`
and creates the :term:`street graph` representation  which can be further processed by other filters, e.g. the :ref:`filter_router`.


Input
=====

- :class:`NodePairList <Common::Types::Street::NodePairList>`
- :class:`TravelDirectionList <Common::Types::Street::TravelDirectionList>`

Output
======

- :class:`Graph <Common::Types::Graph::Graph>`, bidirectional graph representation
- :class:`GraphEdgeMap <Common::Types::Graph::GraphEdgeMap>`, maps from the graph edges to the :term:`street map` segments
- :class:`StreetIndexMap <Common::Types::Graph::StreetIndexMap>`, maps :term:`street map` segments to the corresponding edges and node in th Graph
- :class:`NodeMap <Common::Types::Graph::NodeMap>`, maps the street nodes from :class:`NodePairList <Common::Types::Street::NodePairList>` to the graph nodes in the :class:`Graph <Common::Types::Graph::Graph>`.

Configuration
=============

- None
