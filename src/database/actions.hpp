
#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <variant>

#include <core/base_tags.hpp>

namespace Database::Actions
{
    struct SortByTag;
    struct SortByTimestamp;
    struct SortByID;
    struct GroupAction;
}


namespace Database
{
    typedef std::variant<Actions::SortByTag,
                         Actions::SortByTimestamp,
                         Actions::SortByID,
                         Actions::GroupAction> Action;
}


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

        bool operator==(const SortByTag& other) const
        {
            return tag == other.tag &&
                   sort_order == other.sort_order;
        }

        const TagTypes tag;
        const Qt::SortOrder sort_order;
    };


    struct SortByTimestamp          // Sort by date and time
    {
        SortByTimestamp(const Qt::SortOrder& order = Qt::AscendingOrder):
            sort_order(order)
        {
        }

        bool operator==(const SortByTimestamp& other) const
        {
            return sort_order == other.sort_order;
        }

        const Qt::SortOrder sort_order;
    };


    struct SortByID
    {
        friend auto operator<=>(const SortByID &, const SortByID &) = default;
    };


    struct GroupAction
    {
        GroupAction(std::vector<Action>&& a): actions(a) {}

        const std::vector<Action> actions;

        friend auto operator<=>(const GroupAction &, const GroupAction &) = default;
    };
}


#endif
