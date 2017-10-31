
#include "qmodelindex_comparator.hpp"

#include <QModelIndex>


namespace
{
    std::vector<QModelIndex> getHierarchyOf(const QModelIndex& item)
    {
        std::vector<QModelIndex> result;

        if (item.isValid())
        {
            const QModelIndex parent = item.parent();
            result = getHierarchyOf(parent);
            result.push_back(parent);
        }

        return result;
    }

    bool isLess(const QModelIndex& lhs, const QModelIndex& rhs)
    {
        return  lhs.row() <  rhs.row()
            || (lhs.row() == rhs.row() && (lhs.column() <  rhs.column()
                                       || (lhs.column() == rhs.column() && (lhs.internalId() <  rhs.internalId()
                                                                        || (lhs.internalId() == rhs.internalId() && lhs.model() < rhs.model() )))));
    }
}


bool QModelIndexComparator::operator()(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    std::vector<QModelIndex> lhs_hierarchy = getHierarchyOf(lhs);
    std::vector<QModelIndex> rhs_hierarchy = getHierarchyOf(rhs);

    lhs_hierarchy.push_back(lhs);
    rhs_hierarchy.push_back(rhs);

    const std::size_t s = std::min(lhs_hierarchy.size(), rhs_hierarchy.size());

    bool result = false;
    for (std::size_t i = 0; i < s; i++)
    {
        const QModelIndex& lhs_item = lhs_hierarchy[i];
        const QModelIndex& rhs_item = rhs_hierarchy[i];

        if (isLess(lhs_item, rhs_item))
        {
            result = true;
            break;
        }

        if (isLess(rhs_item, lhs_item))
        {
            result = false;
            break;
        }
    }

    return result;
}
