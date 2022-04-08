
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include "actions.hpp"
#include "photo_types.hpp"
#include "filter.hpp"


namespace Database
{
    struct PHash
    {
        std::uint32_t ph0;
        std::uint32_t ph1;
        std::uint32_t ph2;
        std::uint32_t ph3;

        bool operator==(const PHash &) const = default;
    };

    struct IPhotoOperator
    {
        virtual ~IPhotoOperator() = default;

        virtual bool removePhoto(const Photo::Id &) = 0;
        virtual bool removePhotos(const Filter &) = 0;
        virtual std::vector<Photo::Id> onPhotos(const Filter &, const Action &) = 0;

        /// find all photos matching filters
        virtual std::vector<Photo::Id> getPhotos(const Filter &) = 0;

        // phash operations
        virtual void setPHash(const Photo::Id &, const PHash &) = 0;
        virtual std::optional<PHash> getPHash(const Photo::Id &) = 0;
        virtual bool hasPHash(const Photo::Id &) = 0;
    };
}

#endif
