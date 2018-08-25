
#ifndef GROUP_HPP
#define GROUP_HPP

#include <core/id.hpp>
#include "database_export.h"

namespace Group
{
    DATABASE_EXPORT extern const char Name[16];
    typedef Id<int, Name> Id;
}

struct GroupInfo
{
    enum Role
    {
        None,
        Representative,
        Member,
    };

    enum Type
    {
        Invalid   = 0,
        Animation = 1,
        HDR       = 2,
    };

    Group::Id group_id;      // valid if photo is part of group (if valid, role will be != None)
    Role      role;          // photo's role in group (if None, then group_id is expected to be invalid.)
    Type      type;

    GroupInfo(): group_id(), role(None), type(Invalid) {}
    GroupInfo(const Group::Id& id, Role r, Type t): group_id(id), role(r), type(t)
    {
        assert( (role == None) ^ group_id.valid() );
        assert(type == Animation || type == HDR);
    }

    GroupInfo(const GroupInfo &) = default;

    GroupInfo& operator=(const GroupInfo &) = default;
};

#endif
