
#ifndef IPHOTOS_MANAGER_HPP
#define IPHOTOS_MANAGER_HPP

struct IPhotosManager
{
    virtual ~IPhotosManager() {}
    virtual QByteArray getPhoto(const IPhotoInfo::Ptr &) = 0;
    virtual QByteArray getPhoto(const QString& path) = 0;
};

#endif
