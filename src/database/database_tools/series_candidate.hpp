
#ifndef SERIES_CANDIDATE_HPP_INCLUDED
#define SERIES_CANDIDATE_HPP_INCLUDED

#include <database/photo_data.hpp>

struct GroupCandidate
{
    enum class Type {
        Series,
        HDR,
        Generic,
    };

    Type type;
    std::vector<Photo::DataDelta> members;
};

#endif // SERIES_CANDIDATE_HPP_INCLUDED
