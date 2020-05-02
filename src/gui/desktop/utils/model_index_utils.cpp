
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

    const QModelIndex result = item.row() == 0?
                                QModelIndex():
                                item.sibling(item.row() - 1, 0);

    return result;
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
        QModelIndex parent = item.parent();
        const int siblings = model->rowCount(parent);

        if (item.row() + 1 < siblings)       // is there a next sibling?
            result = next(item);             // go for it
        else
        {                                    // jump out
            while (parent.isValid())
            {
                const QModelIndex result_candidate = next(parent);

                if (result_candidate.isValid())
                {
                    result = result_candidate;
                    break;
                }
                else
                    parent = parent.parent();
            }
        }
    }

    return result;
}


QModelIndex utils::step_in_prev(const QModelIndex& item)
{
    const QAbstractItemModel* model = item.model();

    QModelIndex result;
    QModelIndex previousItem = utils::prev(item);

    if (previousItem.isValid())
    {
        QModelIndex deepestDescendant = previousItem;

        while (deepestDescendant.isValid())
        {
            const int children = model->rowCount(deepestDescendant);

            if (children == 0)  // item has no children, so he is the chosen one
            {
                result = deepestDescendant;
                break;
            }
            else                // otherwise dive into its last child
                deepestDescendant = model->index(children - 1, 0, deepestDescendant);
        }
    }
    else
        result = item.parent();


    return result;
}


QModelIndex utils::first(const QAbstractItemModel& model)
{
    return model.rowCount() == 0?
            QModelIndex():
            model.index(0, 0);
}


QModelIndex utils::last(const QAbstractItemModel& model)
{
    QModelIndex result;
    QModelIndex item = utils::first(model);

    while (item.isValid())
    {
        const QModelIndex parent = item.parent();
        const int siblings = model.rowCount(parent);

        const QModelIndex last_sibling = model.index(siblings - 1, 0, parent);

        result = last_sibling;
        item = model.index(0, 0, last_sibling);
    }

    return result;
}


namespace
{
    QString dump(const QAbstractItemModel& model, const QModelIndex& parent, int depth)
    {
        QString result;

        const int children = model.rowCount(parent);

        for (int i = 0; i < children; i++)
        {
            const QString indent(depth * 2, QChar(' '));
            const QModelIndex child = model.index(i, 0, parent);

            result += QString("%3row: %1, id: %2\n").arg(child.row()).arg(child.internalId()).arg(indent);
            result += dump(model, child, depth + 1);
        }

        return result;
    }
}


QString utils::dump(const QAbstractItemModel& model, const QModelIndex& parent)
{
    return ::dump(model, parent, 0);
}


int utils::getRoleByName(QAbstractItemModel* model, const QString& name)
{
    const auto roles = model->roleNames();

    // Find photo properties role. Model needs to provide it
    const auto it = std::find_if(roles.begin(), roles.end(), [](const auto roleName){
        return roleName == name;
    });

    return it == roles.end()? -1 : it.key();
}
