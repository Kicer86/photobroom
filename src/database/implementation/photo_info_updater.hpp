
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <database/iphoto_info.hpp>

//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final
{
    public:
        PhotoInfoUpdater();
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        static void updateHash(const IPhotoInfo::Ptr &);
        static void updateThumbnail(const IPhotoInfo::Ptr &);
        static void updateTags(const IPhotoInfo::Ptr &);
};

#endif
