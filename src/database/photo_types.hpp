
#ifndef PHOTO_TYPES_HPP
#define PHOTO_TYPES_HPP

#include <deque>
#include <map>
#include <string>

#include <QByteArray>
#include <QMetaType>

#include "database_export.h"

namespace Photo
{
    typedef QByteArray Sha256sum;

    struct DATABASE_EXPORT Id
    {
        typedef int type;

        Id();
        explicit Id(type);
        Id(const Id &) = default;

        Id& operator=(const Id &) = default;
        operator type() const;
        bool operator!() const;
        bool valid() const;
        type value() const;

    private:
        type m_value;
        bool m_valid;
    };

    enum class FlagsE
    {
        StagingArea,
        ExifLoaded,
        Sha256Loaded,
        ThumbnailLoaded,
        GeometryLoaded,
        Role,
    };

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

}

Q_DECLARE_METATYPE(Photo::Id)
Q_DECLARE_METATYPE(std::deque<Photo::Id>)

#endif
