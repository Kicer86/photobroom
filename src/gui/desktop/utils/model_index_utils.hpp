
#ifndef MODEL_INDEX_UTILS_HPP
#define MODEL_INDEX_UTILS_HPP

#include <QModelIndex>

namespace utils
{
    // travel between siblings
    QModelIndex next(const QModelIndex &);
    QModelIndex prev(const QModelIndex &);

    // travel through hierarchy
    QModelIndex step_in_next(const QModelIndex &);
    QModelIndex step_in_prev(const QModelIndex &);

    // accessors
    QModelIndex first(const QAbstractItemModel &);
    QModelIndex last(const QAbstractItemModel &);

    // debug
    QString dump(const QAbstractItemModel &, const QModelIndex & = QModelIndex());

    //
    int getRoleByName(const QAbstractItemModel& model, const QString& name);

    // traverse
    template<typename Op>
    void forEach(const QAbstractItemModel& model, Op op)
    {
        // call for top level item
        op(QModelIndex());

        for (auto it = first(model); it.isValid(); it = step_in_next(it))
            op(it);
    }
}

#endif
