
#ifndef ITHUMBNAILS_ACCESSOR_HPP_INCLUDED
#define ITHUMBNAILS_ACCESSOR_HPP_INCLUDED

#include "photo_data.hpp"


namespace Database
{
    class IThumbnailsAccessor
    {
    public:
        virtual ~IThumbnailsAccessor() = default;
        virtual QImage getThumbnail(const Photo::Id &) = 0;
    };
}

#endif // ITHUMBNAILS_ACCESSOR_HPP_INCLUDED
