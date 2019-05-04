
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include "photo_types.hpp"


namespace Database
{
    struct IPhotoOperator
    {
        virtual ~IPhotoOperator() = default;

        virtual bool removePhotos(const std::vector<Photo::Id> &) = 0;
    };
}

#endif
