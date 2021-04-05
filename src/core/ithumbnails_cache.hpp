
#ifndef IIMAGE_CACHE_HPP
#define IIMAGE_CACHE_HPP

#include <optional>
#include <QImage>


struct IThumbnailsCache
{
    typedef std::tuple<QSize> ThumbnailParameters;

    virtual ~IThumbnailsCache() = default;

    virtual std::optional<QImage> find(const QString &, const ThumbnailParameters &) = 0;
    virtual void store(const QString &, const ThumbnailParameters &, const QImage &) = 0;
};

struct IThumbnailsGenerator
{
    virtual ~IThumbnailsGenerator() = default;

    virtual QImage generate(const QString &, const IThumbnailsCache::ThumbnailParameters& params) = 0;
};

#endif
