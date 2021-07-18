
#ifndef ITHUMBNAILS_GENERATOR_HPP_INCLUDED
#define ITHUMBNAILS_GENERATOR_HPP_INCLUDED

#include <QImage>


class IThumbnailsGenerator
{
public:
    typedef std::tuple<QSize> ThumbnailParameters;

    virtual ~IThumbnailsGenerator() = default;

    virtual QImage generate(const QString &, const ThumbnailParameters& params) = 0;
    virtual QImage generateFrom(const QImage &, const ThumbnailParameters& params) = 0;
};

#endif
