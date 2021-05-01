

#include <database/group.hpp>
#include <database/photo_data.hpp>

struct GroupCandidate
{
    Group::Type type;
    std::vector<Photo::Data> members;
};
