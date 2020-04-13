
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include "photo_types.hpp"
#include "filter.hpp"


namespace Database
{
    class IAction;

    struct IPhotoOperator
    {
        virtual ~IPhotoOperator() = default;

        virtual bool removePhoto(const Photo::Id &) = 0;
        virtual bool removePhotos(const std::vector<IFilter::Ptr> &) = 0;
        virtual std::vector<Photo::Id> onPhotos(const std::vector<IFilter::Ptr> &, IAction &) = 0;

        /// find all photos matching filters
        virtual std::vector<Photo::Id>   getPhotos(const std::vector<IFilter::Ptr> &) = 0;
    };
}

#endif
