
#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <variant>

#include <core/base_tags.hpp>

namespace Database::Actions
{
    struct SortByTag;
    struct SortByTimestamp;
    struct SortByID;
    struct Sort;
    struct GroupAction;
}


namespace Database
{
    typedef std::variant<Actions::SortByTag,
                         Actions::SortByTimestamp,
                         Actions::SortByID,
                         Actions::Sort,
                         Actions::GroupAction> Action;
}


namespace Database::Actions
{
    struct SortByTag
    {
        SortByTag(Tag::Types t, Qt::SortOrder order = Qt::AscendingOrder)
            : tag(t)
            , sort_order(order)
        {

        }

        SortByTag(const SortByTag &) = default;

        friend auto operator<=>(const SortByTag&, const SortByTag&) = default;

        const Tag::Types tag;
        const Qt::SortOrder sort_order;
    };


    struct SortByTimestamp          // Sort by date and time
    {
        SortByTimestamp(const Qt::SortOrder& order = Qt::AscendingOrder):
            sort_order(order)
        {
        }

        friend auto operator<=>(const SortByTimestamp &, const SortByTimestamp &) = default;

        const Qt::SortOrder sort_order;
    };


    struct SortByID
    {
        friend auto operator<=>(const SortByID &, const SortByID &) = default;
    };

    struct Sort
    {
        enum class By
        {
            PHash,
        };

        Sort(By by_, Qt::SortOrder order_ = Qt::AscendingOrder): by(by_), sort_order(order_) {}

        friend auto operator<=>(const Sort &, const Sort &) = default;

        const By by;
        const Qt::SortOrder sort_order;
    };

    struct GroupAction
    {
        explicit GroupAction(std::initializer_list<Action> a): actions(a) {}

        const std::vector<Action> actions;

        friend auto operator<=>(const GroupAction &, const GroupAction &) = default;
    };
}


#endif
