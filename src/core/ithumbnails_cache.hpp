
#ifndef IIMAGE_CACHE_HPP
#define IIMAGE_CACHE_HPP

#include <QImage>


struct IThumbnailsCache
{
    virtual ~IThumbnailsCache() = default;

    virtual std::optional<QImage> find(const QString &, int) = 0;
    virtual void store(const QString &, int, const QImage &) = 0;
};


struct IThumbnailsGenerator
{
    virtual ~IThumbnailsGenerator() = default;

    virtual QImage generate(const QString &, int) = 0;
};


struct IThumbnailUtils
{
    virtual ~IThumbnailUtils() = default;

    virtual IThumbnailsGenerator* generator() = 0;
    virtual IThumbnailsCache* cache() = 0;
};

#endif
