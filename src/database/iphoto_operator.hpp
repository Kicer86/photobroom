
#ifndef IPHOTO_OPERATOR_HPP
#define IPHOTO_OPERATOR_HPP

#include "actions.hpp"
#include "photo_types.hpp"
#include "filter.hpp"


namespace Database
{
    class PHash
    {
    public:
        PHash() = default;
        explicit PHash(qlonglong v): m_hash(v) {}
        explicit PHash(const PHash &) = default;
        explicit PHash(const std::array<std::byte, 8>& v)
        {
            for(unsigned int i = 0; i < v.size(); i++)
            {
                m_hash <<= 8;
                m_hash |= static_cast<std::int64_t>(v[i]);
            }
        }

        bool operator==(const PHash &) const = default;
        PHash& operator=(const PHash &) = default;

        QVariant variant() const
        {
            return static_cast<qlonglong>(m_hash);
        }

    private:
        std::int64_t m_hash = 0;
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
