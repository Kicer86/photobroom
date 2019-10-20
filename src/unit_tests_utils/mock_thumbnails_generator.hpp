
#ifndef MOCK_BACKEND_HPP
#define MOCK_BACKEND_HPP


#include <gmock/gmock.h>

#include <core/ithumbnails_cache.hpp>

struct MockThumbnailsGenerator: IThumbnailsGenerator
{
  MOCK_METHOD(QImage, generate, (const QString &, int), (override));
};

#endif
