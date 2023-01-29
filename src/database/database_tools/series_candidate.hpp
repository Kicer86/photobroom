
#ifndef SERIES_CANDIDATE_HPP_INCLUDED
#define SERIES_CANDIDATE_HPP_INCLUDED

#include <database/explicit_photo_delta.hpp>

struct GroupCandidate
{
    using ExplicitDelta = Photo::ExplicitDelta<Photo::Field::Path>;

    enum class Type {
        Series,
        HDR,
        Generic,
    };

    Type type;
    std::vector<ExplicitDelta> members;
};

#endif
