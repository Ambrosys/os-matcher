#pragma once

#include <cstddef>
#include <functional>
#include <vector>

namespace Core::Graph {

class Graph;

struct Node
{
public:
    bool operator==(Node const & other) const { return this->id_ == other.id_; }

    size_t id() const { return id_; }

private:
    friend class Graph;

    explicit Node(size_t id) : id_{id} {}

    size_t id_;
};

struct Edge
{
public:
    bool operator==(Edge const & other) const { return this->id_ == other.id_; }

    size_t id() const { return id_; }

private:
    friend class Graph;

    explicit Edge(size_t id) : id_{id} {}

    size_t id_;
};

class Graph
{
public:
    virtual ~Graph() = default;

    virtual Node createNode() = 0;
    virtual void remove(Node node) = 0;

    virtual Edge addEdge(Node source, Node target) = 0;
    virtual void remove(Edge edge) = 0;

    virtual bool has(Node node) const = 0;
    virtual bool has(Edge edge) const = 0;

    virtual Node source(Edge edge) const = 0;
    virtual Node target(Edge edge) const = 0;

    virtual std::vector<Edge> outEdges(Node node) const = 0;
    virtual std::vector<Edge> inEdges(Node node) const = 0;

    // virtual size_t nodeCount() const = 0;
    // virtual size_t arcCount() const = 0;

protected:
    Node newNode(size_t id) const { return Node{id}; }
    Edge newEdge(size_t id) const { return Edge{id}; }
};

}  // namespace Core::Graph

namespace std {

template <>
struct hash<Core::Graph::Node>
{
    size_t operator()(Core::Graph::Node const & node) const { return node.id(); }
};

template <>
struct hash<Core::Graph::Edge>
{
    size_t operator()(Core::Graph::Edge const & edge) const { return edge.id(); }
};

}  // namespace std
