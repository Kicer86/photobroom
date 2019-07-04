
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include "photo_types.hpp"
#include "filter.hpp"


namespace Database
{
    struct IPhotoOperator
    {
        virtual ~IPhotoOperator() = default;

        virtual bool removePhoto(const Photo::Id &) = 0;
        virtual bool removePhotos(const std::vector<IFilter::Ptr> &) = 0;
    };
}

#endif
