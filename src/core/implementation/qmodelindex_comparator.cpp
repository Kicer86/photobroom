
#include "qmodelindex_comparator.hpp"

#include <QModelIndex>


namespace
{
    std::vector<QModelIndex> getHierarchyOf(const QModelIndex& item) noexcept
    {
        std::vector<QModelIndex> result;

        const QModelIndex parent = item.parent();

        if (parent.isValid())
            result = getHierarchyOf(parent);

        result.push_back(item);

        return result;
    }

    bool isLess(const QModelIndex& lhs, const QModelIndex& rhs)
    {
        const auto l = std::make_tuple(lhs.row(), lhs.column(), lhs.internalId(), lhs.model());
        const auto r = std::make_tuple(rhs.row(), rhs.column(), rhs.internalId(), rhs.model());

        return l < r;
    }
}


bool QModelIndexComparator::operator()(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    const std::vector<QModelIndex> lhs_hierarchy = getHierarchyOf(lhs);
    const std::vector<QModelIndex> rhs_hierarchy = getHierarchyOf(rhs);
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
