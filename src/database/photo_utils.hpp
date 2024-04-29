
#ifndef PHOTO_UTILS_HPP_INCLUDED
#define PHOTO_UTILS_HPP_INCLUDED

#include <QString>

#include <core/utils.hpp>
#include <database/explicit_photo_delta.hpp>
#include <database_export.h>


namespace Photo
{
    DATABASE_EXPORT const QString& getPath(const Photo::DataDelta &);

    template<typename T>                    // TODO: T is expected to be Photo::ExplicitDelta
    const QString& getPath(const T& data)
    {
        return data.template get<Photo::Field::Path>();
    }

    template<GroupInfo::Role role> bool is(const Photo::DataDelta& data)
    {
        return data.get<Photo::Field::GroupInfo>().role == role;
    }

    template<Photo::Field field>
    bool isLess(const Photo::DataDelta& lhs, const Photo::DataDelta& rhs)
    {
        return lhs.get<field>() < rhs.get<field>();
    }
}


namespace PhotoDelta
{
    template<GroupInfo::Role role> bool is(const Photo::DataDelta& data)
    {
        return data.get<Photo::Field::GroupInfo>().role == role;
    }

    template<Photo::Field field>
    bool isLess(const Photo::DataDelta& lhs, const Photo::DataDelta& rhs)
    {
        return lhs.get<field>() < rhs.get<field>();
    }
}


namespace PhotoExplicitDelta
{
    template<GroupInfo::Role role, typename T> bool is(const T& data)           // TODO: T is expected to be Photo::ExplicitDelta
    {
        return data.template get<Photo::Field::GroupInfo>().role == role;
    }
}

#endif
