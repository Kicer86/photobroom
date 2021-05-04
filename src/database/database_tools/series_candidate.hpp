
#ifndef SERIES_CANDIDATE_HPP_INCLUDED
#define SERIES_CANDIDATE_HPP_INCLUDED

#include <database/group.hpp>
#include <database/photo_data.hpp>

struct GroupCandidate
{
    Group::Type type;
    std::vector<Photo::Data> members;
};

#endif // SERIES_CANDIDATE_HPP_INCLUDED
