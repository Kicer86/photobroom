
#ifndef IIMAGE_CACHE_HPP
#define IIMAGE_CACHE_HPP

#include <optional>
#include <QImage>


struct IThumbnailsCache
{
    virtual ~IThumbnailsCache() = default;

    virtual std::optional<QImage> find(const QString &, int) = 0;
    virtual void store(const QString &, int, const QImage &) = 0;
};


struct IThumbnailsGenerator
{
    enum class Mode
    {
        Height,
        Width,
        Auto,
    };

    virtual ~IThumbnailsGenerator() = default;

    virtual QImage generate(const QString &, int) = 0;
    virtual QImage generate(const QString &, int, Mode) = 0;
};

#endif
