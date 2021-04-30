/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <Core/Graph/Graph.h>

#include <boost/iterator/iterator_facade.hpp>

namespace Core::Graph::Routing {

class PathViewImpl
{
public:
    virtual ~PathViewImpl() {}
    virtual Core::Graph::Edge edge() const = 0;
    virtual double cost() const = 0;
    virtual PathViewImpl * previous() const = 0;
};

class PathViewIterator;

class PathView
{
public:
    using Iterator = PathViewIterator;

    PathView(PathViewImpl * impl);

    Core::Graph::Edge edge() const;
    double cost() const;

    PathView back() const;
    PathView front() const;
    PathView previous() const;
    Iterator begin() const;
    Iterator end() const;
    bool empty() const;
    size_t size() const;

private:
    PathViewImpl * impl_;
};

class PathViewIterator : public boost::iterator_facade<PathViewIterator, PathView, boost::forward_traversal_tag, PathView>
{
public:
    explicit PathViewIterator(PathViewImpl * path);

private:
    friend class boost::iterator_core_access;

    void increment();
    bool equal(PathViewIterator const & other) const;
    PathView dereference() const;

    PathViewImpl * path_;
    // PathView view_;
};

std::ostream & operator<<(std::ostream & os, PathView pathView);

}  // namespace Core::Graph::Routing
