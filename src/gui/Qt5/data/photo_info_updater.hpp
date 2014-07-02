
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <core/photo_info.hpp>

//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final
{
    public:
        PhotoInfoUpdater();
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        static void updateHash(const PhotoInfo::Ptr &);
        static void updateThumbnail(const PhotoInfo::Ptr &);
        static void updateTags(const PhotoInfo::Ptr &);
};

#endif
