
#ifndef PHOTO_UTILS_HPP_INCLUDED
#define PHOTO_UTILS_HPP_INCLUDED

#include <QString>

#include <core/utils.hpp>
#include <database/photo_data.hpp>
#include <database_export.h>


namespace Photo
{
    inline auto& getId = extract<Photo::Data, Photo::Id, &Photo::Data::id>;

    DATABASE_EXPORT const QString& getPath(const Photo::Data &);
    DATABASE_EXPORT const QString& getPath(const Photo::DataDelta &);

    template<GroupInfo::Role role> bool is(const Photo::Data& data)
    {
        return data.groupInfo.role == role;
    }

    template<Photo::Field field>
    bool isLess(const Photo::DataDelta& lhs, const Photo::DataDelta& rhs)
    {
        return lhs.get<field>() < rhs.get<field>();
    }
}

#endif // PHOTO_UTILS_HPP_INCLUDED
