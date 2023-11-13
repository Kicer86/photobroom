
#ifndef BACKEND_UTILS_HPP_INCLUDED
#define BACKEND_UTILS_HPP_INCLUDED

#include "ibackend.hpp"
#include "iphoto_operator.hpp"

namespace Database
{
    template<Photo::Field... fields>
    std::vector<Photo::ExplicitDelta<fields...>> getPhotoDelta(IBackend& backend, const Filter& filter = {})
    {
        const auto ids = backend.photoOperator().getPhotos(filter);
        std::vector<Photo::ExplicitDelta<fields...>> photos;

        std::ranges::transform(ids, std::back_inserter(photos), [&backend](const auto& id)
        {
            return backend.getPhotoDelta<fields...>(id);
        });

        return photos;
    }

}


#endif
