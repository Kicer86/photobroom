
#ifndef PHOTO_UTILS_HPP_INCLUDED
#define PHOTO_UTILS_HPP_INCLUDED

#include <QString>

#include <database/photo_data.hpp>
#include <database_export.h>


namespace Photo
{
    DATABASE_EXPORT const QString& getPath(const Photo::Data &);
    DATABASE_EXPORT const QString& getPath(const Photo::DataDelta &);

    template<GroupInfo::Role role> bool is(const Photo::Data& data)
    {
        return data.groupInfo.role == role;
    }
}

#endif // PHOTO_UTILS_HPP_INCLUDED
