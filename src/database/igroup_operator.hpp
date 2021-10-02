
#ifndef IGROUP_OPERATOR_HPP
#define IGROUP_OPERATOR_HPP

#include "group.hpp"
#include "photo_types.hpp"

namespace Database
{
    struct IGroupOperator
    {
        virtual Group::Id addGroup(const Photo::Id& representative_photo, Group::Type) = 0;
        virtual Photo::Id removeGroup(const Group::Id &) = 0;
        virtual Group::Type type(const Group::Id &) const = 0;
        virtual std::vector<Photo::Id> membersOf(const Group::Id &) const = 0;
        virtual std::vector<Group::Id> listGroups() const = 0;
    };
}

#endif
