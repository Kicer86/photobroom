
#ifndef MOCK_BACKEND_HPP
#define MOCK_BACKEND_HPP


#include <gmock/gmock.h>

#include <core/thumbnail_manager.hpp>

struct MockThumbnailGenerator: AThumbnailGenerator
{
  MOCK_METHOD3(run,
      void(const QString &, int, std::unique_ptr<ICallback>));

  using AThumbnailGenerator::ICallback;
};

#endif
