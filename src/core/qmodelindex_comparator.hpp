
#ifndef QMODELINDEX_COMPARATOR_HPP
#define QMODELINDEX_COMPARATOR_HPP

class QModelIndex;

struct QModelIndexComparator
{
    bool operator()(const QModelIndex &, const QModelIndex &) const;
};

#endif // QMODELINDEX_COMPARATOR_HPP
