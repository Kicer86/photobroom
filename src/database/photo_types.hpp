
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

}

Q_DECLARE_METATYPE(Photo::Id)
Q_DECLARE_METATYPE(std::vector<Photo::Id>)

#endif
