
#ifndef MOCK_THUMBNAILS_GENERATOR_HPP
#define MOCK_THUMBNAILS_GENERATOR_HPP


#include <gmock/gmock.h>

#include <core/ithumbnails_generator.hpp>

struct MockThumbnailsGenerator: IThumbnailsGenerator
{
    MOCK_METHOD(QImage, generate, (const QString &, const ThumbnailParameters& params), (override));
    MOCK_METHOD(QImage, generateFrom, (const QImage &, const ThumbnailParameters& params), (override));
};

#endif
