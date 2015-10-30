
#ifndef PHOTO_TYPES_HPP
#define PHOTO_TYPES_HPP

#include <string>
#include <map>

#include "database_export.h"

namespace Photo
{
    typedef std::string Sha256sum;

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

#endif
