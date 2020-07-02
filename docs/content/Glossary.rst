========
Glossary
========

General
=======

.. glossary::
    :sorted:

    candidate

    street map
        Refers to a data source with street map data or to the map itself as described by the data.
        A street map consist at least of a set of street segment geometries, connected by node points.

    track
        Refers to a data source with track data or to the track itself as described by the data.
        A track consists at least of an ordered list of geo points with timestamps.
        The track might consist of additional data as velocity, acceleration, direction etc.

    valid navigation route
        A valid navigation route is a route which you could navigate.

        Currently the following constraints are considered:
            - Allowed travel direction
            - Existing street intersections
            - Self intersections (optional)
            - Velocity difference (optional; for street matching)
            - Angular deviation (optional; for street matching)

        .. seealso:: :class:`struct RouteRestrictions <AppComponents::Common::Filter::Routing::RouteRestrictions>`

    sampling point
        A track point, which at least has one :term:`candidate<sampling point candidate>` on a :term:`street segment`.

        .. seealso:: :ref:`sampling_point_finder`

    candidate
    sampling point candidate
        A track point projected on a :term:`street segment`.
        The projection has to meet specific requirements in order to be considered a candidate.

        Candidates are ordered according to (in decreasing importance):
        * distance
        * heading difference
        * The first is the best.

    originID
        The ID of a street