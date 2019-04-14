
#ifndef IGROUP_OPERATOR_HPP
#define IGROUP_OPERATOR_HPP

#include "group.hpp"
#include "photo_types.hpp"

namespace Database
{
    struct IGroupOperator
    {
        virtual Group::Id addGroup(const Photo::Id &, GroupInfo::Type) = 0;
    };
}

#endif
