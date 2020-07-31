
#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <variant>

#include <core/base_tags.hpp>

namespace Database::Actions
{
    struct SortAction
    {
        SortAction(TagTypes t, Qt::SortOrder order = Qt::AscendingOrder)
            : tag(t)
            , sort_order(order)
        {

        }

        SortAction(const SortAction &) = default;

        const TagTypes tag;
        const Qt::SortOrder sort_order;
    };
}

namespace Database
{
    typedef std::variant<Actions::SortAction> Action;
}

#endif
