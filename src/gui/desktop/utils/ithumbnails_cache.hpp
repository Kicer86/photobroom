
#ifndef IIMAGE_CACHE_HPP
#define IIMAGE_CACHE_HPP

#include <optional>
#include <QImage>

#include <database/photo_types.hpp>


struct IThumbnailsCache
{
    typedef std::tuple<QSize> ThumbnailParameters;

    virtual ~IThumbnailsCache() = default;

    virtual std::optional<QImage> find(const Photo::Id &, const ThumbnailParameters &) = 0;
    virtual void store(const Photo::Id &, const ThumbnailParameters &, const QImage &) = 0;
};

#endif
