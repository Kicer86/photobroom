
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include <optional>

#include "actions.hpp"
#include "filter.hpp"
#include "photo_data.hpp"
#include "photo_types.hpp"
#include "explicit_photo_delta.hpp"


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

        /**
         * @brief fetch photos data
         *
         * @param filter photos to match
         * @param fields list of photo details to read
         *
         * @return vector of structures containing chosen fields of filtered photos
         */
        virtual std::vector<Photo::DataDelta> fetchData(const Filter& filter, const std::set<Photo::Field>& fields = Photo::AllFields) = 0;

        /**
         * @brief fetch photos data
         *
         * @param filter photos to match
         * @param fields list of photo details to read
         *
         * @return vector of explicit structures containing chosen fields of filtered photos
         *
         */
        template<Photo::Field... fields>
        std::vector<Photo::ExplicitDelta<fields...>> fetchData(const Filter& filter)
        {
            std::set<Photo::Field> f;

            auto append = [&f](Photo::Field field) { f.insert(field); };
            (..., append(fields));

            return EDV<Photo::ExplicitDelta<fields...>>(fetchData(filter, f));
        }

        // phash operations
        virtual void setPHash(const Photo::Id &, const Photo::PHashT &) = 0;
        virtual std::optional<Photo::PHashT> getPHash(const Photo::Id &) = 0;
        virtual bool hasPHash(const Photo::Id &) = 0;
    };
}

#endif
