
#ifndef QMODELINDEX_COMPARATOR_HPP
#define QMODELINDEX_COMPARATOR_HPP

class QModelIndex;

#include "core_export.h"

struct CORE_EXPORT QModelIndexComparator
{
    bool operator()(const QModelIndex &, const QModelIndex &) const;
};

#endif // QMODELINDEX_COMPARATOR_HPP
