
#ifndef IGROUP_OPERATOR_HPP
#define IGROUP_OPERATOR_HPP

#include "group.hpp"
#include "photo_types.hpp"

namespace Database
{
    struct IGroupOperator
    {
        virtual Group::Id addGroup(const Photo::Id& representative_photo, GroupInfo::Type) = 0;
        virtual Photo::Id removeGroup(const Group::Id) = 0;
    };
}

#endif
