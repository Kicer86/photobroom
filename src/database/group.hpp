
#ifndef GROUP_HPP
#define GROUP_HPP

#include <core/id.hpp>
#include "database_export.h"

namespace Group
{
    using Id = Id<int, struct database_tag>;

    enum Type
    {
        Invalid   = 0,
        Animation = 1,
        HDR       = 2,
        Generic   = 3,      // bunch of photos
        Collage   = 4,
    };
}

struct GroupInfo
{
    enum Role
    {
        None           = 0,
        Representative = 1,
        Member         = 2,
    };

    Group::Id group_id;      // valid if photo is part of group (if valid, role will be != None)
    Role      role;          // photo's role in group (if None, then group_id is expected to be invalid.)

    GroupInfo(): group_id(), role(None) {}
    GroupInfo(const Group::Id& id, Role r): group_id(id), role(r)
    {
        assert( (role == None) ^ group_id.valid() );
    }

    GroupInfo(const GroupInfo &) = default;

    GroupInfo& operator=(const GroupInfo &) = default;

    friend auto operator<=>(const GroupInfo&, const GroupInfo&) = default;
};

#endif
