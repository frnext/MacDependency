============================
|(logo)|__ ``read_graphml``
============================

.. |(logo)| image:: ../../../boost.png
   :align: middle
   :alt: Boost

__ ../../../index.htm

::

  void read_graphml(std::istream& in, MutableGraph& graph,
                    dynamic_properties& dp);

 
The ``read_graphml`` function interprets a graph described using the
graphml_ format and builds a BGL graph that captures that
description.  Using this function, you can initialize a graph using
data stored as text. 

The graphml format can specify both directed and undirected graphs, and
``read_graphml`` differentiates between the two. One must pass
``read_graphml`` an undirected graph when reading an undirected graph;
the same is true for directed graphs. Furthermore, ``read_graphml``
will throw an exception if it encounters parallel edges and cannot add
them to the graph.

To handle attributes expressed in the graphml format, ``read_graphml``
takes a dynamic_properties_ object and operates on its collection of
property maps.  The reader passes all the properties encountered to
this object, using the graphml attribute names as the property keys,
and with the appropriate C++ type based on the graphml attribute type
definition.

Requirements:
 - The type of the graph must model the `Mutable Graph`_ concept.
 - The type of the iterator must model the `Multi-Pass Iterator`_
   concept.
 - The property map value types must be default-constructible.


.. contents::

Where Defined
-------------
``<boost/graph/graphml.hpp>``

Exceptions
----------

::

  struct graph_exception : public std::exception {
    virtual ~graph_exception() throw();
    virtual const char* what() const throw() = 0;
  };

  struct bad_parallel_edge : public graph_exception {
    std::string from;
    std::string to;

    bad_parallel_edge(const std::string&, const std::string&);
    virtual ~bad_parallel_edge() throw();
    const char* what() const throw();
  };

  struct directed_graph_error : public graph_exception {
    virtual ~directed_graph_error() throw();
    virtual const char* what() const throw();
  };

  struct undirected_graph_error : public graph_exception {
    virtual ~undirected_graph_error() throw();
    virtual const char* what() const throw();
  };

  struct parse_error : public graph_exception {
    parse_error(const std::string&);
    virtual ~parse_error() throw() {}
    virtual const char* what() const throw();
    std::string statement;
  };

Under certain circumstances, ``read_graphml`` will throw one of the
above exceptions.  The three concrete exceptions can all be caught
using the general ``graph_exception`` moniker when greater precision
is not needed.  In addition, all of the above exceptions derive from
the standard ``std::exception`` for even more generalized error
handling.

The ``bad_parallel_edge`` exception is thrown when an attempt to add a
parallel edge to the supplied MutableGraph fails.  The graphml format
supports parallel edges, but some BGL-compatible graph types do not.
One example of such a graph is ``boost::adjacency_list<setS,vecS>``,
which allows at most one edge can between any two vertices.


The ``directed_graph_error`` exception occurs when an undirected graph
type is passed to ``read_graph`` but the textual representation of the
graph is directed, as indicated by the ``edgedefault="directed"``
graph attribute in the graphml format.

The ``undirected_graph_error`` exception occurs when a directed graph
type is passed to ``read_graph`` but the textual representation of the
graph is undirected, as indicated by the ``edgedefault="undirected"``
graph attribute in the graphml format.


Building the graphml reader
-----------------------------
To use the graphml reader, you will need to build and link against
the "bgl-graphml" library. The library can be built by following the
`Boost Jam Build Instructions`_ for the subdirectory ``libs/graph/build``.


Notes
-----

 - On successful reading of a graph, every vertex and edge will have
   an associated value for every respective edge and vertex property
   encountered while interpreting the graph.  These values will be set
   using the ``dynamic_properties`` object.  Some properties may be
   ``put`` multiple times during the course of reading in order to
   ensure the graphml semantics.  Those edges and vertices that are
   not explicitly given a value for a property (and that property has
   no default) will be given the default constructed value of the
   value type.  **Be sure that property map value types are default
   constructible.**

 - Nested graphs are supported as long as they are exactly of the same
   type as the root graph, i.e., are also directed or undirected. Note
   that since nested graphs are not directly supported by BGL, they
   are in fact completely ignored when building the graph, and the
   internal vertices or edges are interpreted as belonging to the root
   graph.

 - Hyperedges and Ports are not supported.

See Also
--------

write_graphml_


Future Work
-----------

 - Better expat error detection.
 

.. _Graphml: http://graphml.graphdrawing.org/

.. _`Mutable Graph`: MutableGraph.html
.. _`Multi-Pass Iterator`: ../../iterator/index.html
.. _dynamic_properties: ../../property_map/doc/dynamic_property_map.html
.. _write_graphml: write_graphml.html
.. _Boost Jam Build Instructions: ../../../more/getting_started.html#Build_Install