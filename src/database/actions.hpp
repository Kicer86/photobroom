
#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <variant>

#include <core/base_tags.hpp>

namespace Database::Actions
{
    struct SortByTag;
    struct Sort;
    struct GroupAction;
}


namespace Database
{
    typedef std::variant<Actions::SortByTag,
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

    struct Sort
    {
        enum class By
        {
            PHash,
            Timestamp,                  // Sort by date and time
            ID,
        };

        Sort(By by_, Qt::SortOrder order_ = Qt::AscendingOrder): by(by_), order(order_) {}

        friend auto operator<=>(const Sort &, const Sort &) = default;

        const By by;
        const Qt::SortOrder order;
    };

    struct GroupAction
    {
        explicit GroupAction(std::initializer_list<Action> a): actions(a) {}

        const std::vector<Action> actions;

        friend auto operator<=>(const GroupAction &, const GroupAction &) = default;
    };
}


#endif
