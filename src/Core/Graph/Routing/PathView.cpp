/*
 * SPDX-FileCopyrightText: © 2018 Ambrosys GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Core/Graph/Routing/PathView.h>

#include <ostream>

namespace Core::Graph::Routing {

PathViewIterator::PathViewIterator(PathViewImpl * path) : path_{path}
{
}

void PathViewIterator::increment()
{
    path_ = path_->previous();
}

bool PathViewIterator::equal(PathViewIterator const & other) const
{
    return this->path_ == other.path_;
}

PathView PathViewIterator::dereference() const
{
    return path_;
}

PathView::PathView(PathViewImpl * impl) : impl_{impl}
{
}

Core::Graph::Edge PathView::edge() const
{
    return impl_->edge();
}

double PathView::cost() const
{
    return impl_->cost();
}

PathView PathView::back() const
{
    if (impl_ == nullptr)
        return PathView(nullptr);
    auto current = impl_;
    while (current->previous() != nullptr)
        current = current->previous();
    return PathView(current);
}

PathView PathView::front() const
{
    return PathView(impl_);
}

PathView::Iterator PathView::begin() const
{
    return Iterator(impl_);
}

PathView PathView::previous() const
{
    return PathView(impl_->previous());
}

PathView::Iterator PathView::end() const
{
    return Iterator(nullptr);
}

bool PathView::empty() const
{
    return impl_ == nullptr;
}

size_t PathView::size() const
{
    // TODO(jg): replace with std::distance
    size_t count = 0;
    auto current = impl_;
    while (current != nullptr)
    {
        current = current->previous();
        ++count;
    }
    return count;
}

std::ostream & operator<<(std::ostream & os, PathView pathView)
{
    for (auto pv : pathView)
        os << pv.edge().id() << (pv.previous().empty() ? "" : " <- ");
    return os;
}

}  // namespace Core::Graph::Routing
