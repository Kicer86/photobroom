
#ifndef IPHOTOS_MANAGER_HPP
#define IPHOTOS_MANAGER_HPP

class QString;

struct IPhotosManager
{
    virtual ~IPhotosManager() = default;

    virtual QByteArray getPhoto(const QString& path) const = 0;
};

#endif
