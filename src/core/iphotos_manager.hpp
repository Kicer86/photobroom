
#ifndef IPHOTOS_MANAGER_HPP
#define IPHOTOS_MANAGER_HPP

// TODO: core should be independent

#include <database/iphoto_info.hpp>

struct IPhotosManager
{
    virtual ~IPhotosManager();
    virtual QByteArray getPhoto(const IPhotoInfo::Ptr &) = 0;
    virtual QByteArray getPhoto(const QString& path) = 0;

    virtual QImage getThumbnail(const QString& path) = 0;
};

#endif
