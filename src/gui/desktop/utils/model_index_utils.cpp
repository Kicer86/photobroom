
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


QModelIndex utils::step_in_next(const QModelIndex& item)
{
    QModelIndex result;
    const QAbstractItemModel* model = item.model();

    const int children = model->rowCount(item);

    if (children > 0)
        result = model->index(0, 0, item);
    else
    {
        const QModelIndex parent = item.parent();
        const int siblings = model->rowCount(parent);

        if (item.row() + 1 < siblings)       // is there a next sibling?
            result = next(item);             // go for it
        else
            result = next(parent);           // go to parent's sibling
    }

    return result;
}


QModelIndex utils::step_in_prev(const QModelIndex& item)
{
    return QModelIndex();
}
