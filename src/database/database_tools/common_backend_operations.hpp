
#ifndef COMMON_BACKEND_OPERATIONS_HPP_INCLUDED
#define COMMON_BACKEND_OPERATIONS_HPP_INCLUDED

#include <database/ibackend.hpp>


namespace Database
{
    template<typename T> requires std::input_iterator<T> && std::is_same_v<typename T::value_type, Photo::Id>
    std::vector<Photo::Data> fetchPhotoData(Database::IBackend& backend, T first, T last)
    {
        std::vector<Photo::Data> data;
        data.reserve(std::distance(first, last));

        for(; first != last; ++first)
            data.push_back(backend.getPhoto(*first));

        return data;
    }
}

#endif
