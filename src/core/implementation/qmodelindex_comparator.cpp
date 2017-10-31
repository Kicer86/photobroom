
#include "qmodelindex_comparator.hpp"

#include <QModelIndex>


bool QModelIndexComparator::operator()(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return  lhs.row() <  rhs.row()
        || (lhs.row() == rhs.row() && (lhs.column() <  rhs.column()
                                    || (lhs.column() == rhs.column() && (lhs.internalId() <  rhs.internalId()
                                                                    || (lhs.internalId() == rhs.internalId() && lhs.model() < rhs.model() )))));
}
