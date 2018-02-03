
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

    // debug
    QString dump(const QAbstractItemModel &, const QModelIndex & = QModelIndex());
}

#endif
