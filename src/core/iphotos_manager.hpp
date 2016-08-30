
#ifndef IPHOTOS_MANAGER_HPP
#define IPHOTOS_MANAGER_HPP

// TODO: core should be independent

#include <database/iphoto_info.hpp>

struct IPhotosManager
{
    virtual ~IPhotosManager() = default;

    virtual QByteArray getPhoto(const IPhotoInfo::Ptr &) const = 0;
    virtual QByteArray getPhoto(const QString& path) const = 0;
};

#endif
