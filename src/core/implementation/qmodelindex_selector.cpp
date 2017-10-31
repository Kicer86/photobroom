
#include "qmodelindex_selector.hpp"

#include <map>
#include <set>

#include "containers_utils.hpp"
#include "map_iterator.hpp"
#include "qmodelindex_comparator.hpp"


std::vector<QModelIndex> QModelIndexSelector::listAllBetween(const QModelIndex& from, const QModelIndex& to)
{
    std::map<QModelIndex, std::set<QModelIndex, QModelIndexComparator>> sub_selections;       // selection within parent

    // group selected items by parent
    for(QModelIndex item = from; ; item = item.sibling(item.row() + 1, 0))
    {
        if (item.isValid() == false)
            break;

        const QModelIndex parent = item.parent();

        sub_selections[parent].insert(item);

        if (item == to)
            break;
    }

    std::vector<QModelIndex> linear_selection;

    typedef value_map_iterator<decltype(sub_selections)> ValueIt;
    for(ValueIt it(sub_selections.begin()); it != ValueIt(sub_selections.end()); ++it)
    {
        const std::set<QModelIndex, QModelIndexComparator>& set = *it;

        // indexes are sorted by position (see QModelIndexComparator), so here we can refer to first and last one easily
        const QModelIndex& first = front(set);
        const QModelIndex& last  = back(set);

        for(QModelIndex item = first; item != last ; item = item.sibling(item.row() + 1, 0))
            linear_selection.push_back(item);

        linear_selection.push_back(last);
    }

    return linear_selection;
}
