
#ifndef PHOTO_TYPES_HPP
#define PHOTO_TYPES_HPP

#include <vector>
#include <map>
#include <string>

#include <QByteArray>
#include <QMetaType>

#include "database_export.h"

#include "core/id.hpp"

namespace Photo
{
    typedef QByteArray Sha256sum;

    DATABASE_EXPORT extern const char Name[];
    typedef Id<int, Name> Id;

    enum class FlagsE
    {
        StagingArea,
        ExifLoaded,
        Sha256Loaded,
        ThumbnailLoaded,
        GeometryLoaded,
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
Q_DECLARE_METATYPE(std::vector<Photo::Id>)

#endif
