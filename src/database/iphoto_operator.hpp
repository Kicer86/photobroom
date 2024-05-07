
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include <optional>

#include "actions.hpp"
#include "filter.hpp"
#include "photo_data.hpp"
#include "photo_types.hpp"


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
        virtual std::vector<Photo::DataDelta> fetchData(const Filter &) = 0;

        // phash operations
        virtual void setPHash(const Photo::Id &, const Photo::PHashT &) = 0;
        virtual std::optional<Photo::PHashT> getPHash(const Photo::Id &) = 0;
        virtual bool hasPHash(const Photo::Id &) = 0;
    };
}

#endif
