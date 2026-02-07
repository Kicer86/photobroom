
#include "qmodelindex_selector.hpp"

#include <cassert>
#include <map>
#include <set>

#include "qmodelindex_comparator.hpp"

namespace
{
    QModelIndex next(const QModelIndex& item)
    {
        const QModelIndex sibling = item.sibling(item.row() + 1, 0);

        return sibling;
    }
}

std::vector<QModelIndex> QModelIndexSelector::listAllBetween(const QModelIndex& from, const QModelIndex& to)
{
    std::vector<QModelIndex> result;

    QModelIndex item = from;

    while (true)
    {
        // should not happen, but if it does, do not hang
        assert(item.isValid());
        if (item.isValid() == false)
            break;

        result.push_back(item);

        // finish?
        if (item == to)
            break;

        // go to children if possible
        const QAbstractItemModel* model = item.model();
        const bool hasChildren = model->hasChildren(item);
        if (hasChildren)
        {
            item = model->index(0, 0, item);
            continue;
        }

        // go to sibling if possible
        const QModelIndex sibling = next(item);
        if (sibling.isValid())
        {
            item = sibling;
            continue;
        }

        // no next sibling, no children, go to parent's sibling

        const QModelIndex parent = item.parent();
        const QModelIndex parents_sibling = next(parent);
        item = parents_sibling;
    }

    return result;
}
