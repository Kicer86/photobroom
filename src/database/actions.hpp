
#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <variant>

#include <core/base_tags.hpp>

namespace Database::Actions
{
    struct SortByTag
    {
        SortByTag(TagTypes t, Qt::SortOrder order = Qt::AscendingOrder)
            : tag(t)
            , sort_order(order)
        {

        }

        SortByTag(const SortByTag &) = default;
        bool operator<=>(const SortByTag &) const = default;

        const TagTypes tag;
        const Qt::SortOrder sort_order;
    };


    struct SortByTimestamp          // Sort by date and time
    {
        SortByTimestamp(const Qt::SortOrder& order = Qt::AscendingOrder):
            sort_order(order)
        {
        }

        const Qt::SortOrder sort_order;

        bool operator<=>(const SortByTimestamp &) const = default;
    };
}

namespace Database
{
    typedef std::variant<Actions::SortByTag, Actions::SortByTimestamp> Action;
}

#endif
