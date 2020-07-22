.. _filter_graphbuilder:

============
GraphBuilder
============

This filter takes part of the intrnal representation of a :term:`street map`
and creates the :term:`street graph` representation  which can be further processed by other filters, e.g. the :ref:`filter_router`.


Input
=====

- :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>`
- :class:`TravelDirectionList <AppComponents::Common::Types::Street::TravelDirectionList>`

Output
======

- :class:`Graph <AppComponents::Common::Types::Graph::Graph>`, bidirectional graph representation
- :class:`GraphEdgeMap <AppComponents::Common::Types::Graph::GraphEdgeMap>`, maps from the graph edges to the :term:`street map` segments
- :class:`StreetIndexMap <AppComponents::Common::Types::Graph::StreetIndexMap>`, maps :term:`street map` segments to the corresponding edges and node in th Graph
- :class:`NodeMap <AppComponents::Common::Types::Graph::NodeMap>`, maps the street nodes from :class:`NodePairList <AppComponents::Common::Types::Street::NodePairList>` to the graph nodes in the :class:`Graph <AppComponents::Common::Types::Graph::Graph>`.

Configuration
=============

- None
