
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include <optional>

#include "actions.hpp"
#include "photo_types.hpp"
#include "filter.hpp"


namespace Database
{
    struct IPhotoOperator
    {
        virtual ~IPhotoOperator() = default;

        virtual bool removePhoto(const Photo::Id &) = 0;
        virtual bool removePhotos(const Filter &) = 0;
        virtual std::vector<Photo::Id> onPhotos(const Filter &, const Action &) = 0;

        /// find all photos matching filters
        virtual std::vector<Photo::Id> getPhotos(const Filter &) = 0;

        // phash operations
        virtual void setPHash(const Photo::Id &, const Photo::PHash &) = 0;
        virtual std::optional<Photo::PHash> getPHash(const Photo::Id &) = 0;
        virtual bool hasPHash(const Photo::Id &) = 0;
    };
}

#endif
