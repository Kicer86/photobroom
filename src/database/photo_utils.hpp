
#ifndef PHOTO_UTILS_HPP_INCLUDED
#define PHOTO_UTILS_HPP_INCLUDED

#include <QString>

#include <database/photo_data.hpp>
#include <database_export.h>


namespace Photo
{
    DATABASE_EXPORT const QString& getPath(const Photo::Data &);
}

#endif // PHOTO_UTILS_HPP_INCLUDED
