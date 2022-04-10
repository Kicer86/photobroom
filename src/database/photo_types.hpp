
#ifndef PHOTO_TYPES_HPP
#define PHOTO_TYPES_HPP

#include <vector>
#include <map>
#include <string>

#include <QByteArray>
#include <QMetaType>
#include <QVariant>

#include "database_export.h"

#include "core/id.hpp"

namespace Photo
{
    Q_NAMESPACE_EXPORT(DATABASE_EXPORT)

    using Id = Id<int, struct photo_tag>;

    enum class FlagsE
    {
        StagingArea,
        ExifLoaded,
        GeometryLoaded,
    };
    Q_ENUM_NS(FlagsE)

    enum class Roles
    {
        RegularPhoto,
        Representative,
    };

    typedef std::map<FlagsE, int> FlagValues;

    struct IdHash
    {
        std::size_t operator()(const Id& key) const
        {
            return std::hash<Id::type>()(key.value());
        }
    };

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
}

Q_DECLARE_METATYPE(Photo::Id)
Q_DECLARE_METATYPE(std::vector<Photo::Id>)

#endif
