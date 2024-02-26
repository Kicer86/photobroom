
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <unit_tests_utils/empty_logger.hpp>
#include <unit_tests_utils/printers.hpp>
#include "image_aligner.hpp"


using testing::ElementsAre;


TEST(ImageAlignerTest, progress)
{
    EmptyLogger logger;

    const QStringList photos =
    {
        "alterd_images/img1_.png",
        "alterd_images/img1_.png",
        "alterd_images/img1_.png",
        "alterd_images/img1_.png",
        "alterd_images/img1_.png"
    };

    std::vector<std::pair<int, int>> steps;
    auto progress = [&](int current, int all)
    {
        steps.emplace_back(current, all);
    };

    const auto alignedImages = ImageAligner(photos, logger).registerProgress(progress).align();
    ASSERT_TRUE(alignedImages);

    alignedImages->forEachImage([&](const auto &) { });

    EXPECT_THAT(
        steps,
        ElementsAre
        (
            std::pair(0, 5),
            std::pair(1, 5),
            std::pair(2, 5),
            std::pair(3, 5),
            std::pair(4, 5),
            std::pair(5, 5)
        )
    );
}
