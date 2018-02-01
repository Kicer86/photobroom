
#include "model_index_utils.hpp"

#include <cassert>

QModelIndex utils::next(const QModelIndex& item)
{
    assert(item.isValid());
    assert(item.column() == 0);

    return item.sibling(item.row() + 1, 0);
}


QModelIndex utils::prev(const QModelIndex& item)
{
    assert(item.isValid());
    assert(item.column() == 0);

    return item.sibling(item.row() - 1, 0);
}

