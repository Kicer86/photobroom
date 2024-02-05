
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ranges>

#include "containers_utils.hpp"
#include "image_aligner.hpp"

using testing::ElementsAre;


namespace
{
    std::vector<long> transformationValues(const cv::Mat& mat)
    {
        return {
            std::lround(mat.at<float>(0, 0)),
            std::lround(mat.at<float>(0, 1)),
            std::lround(mat.at<float>(0, 2)),
            std::lround(mat.at<float>(1, 0)),
            std::lround(mat.at<float>(1, 1)),
            std::lround(mat.at<float>(1, 2))
        };
    }
}


TEST(ImageAlignerTest, mixOfImagesOfTheSameSize)
{
    const QStringList photos =
    {
        "alterd_images/img1_.png",
        "alterd_images/img1_moved_left.png",
        "alterd_images/img1_moved_right.png",
        "alterd_images/img1_rotated_negative.png",
        "alterd_images/img1_rotated.png"
    };

    ImageAligner aligner;

    const auto transformations = aligner.calculateTransformations(photos);

    auto transformationV = range_to<std::vector<std::vector<long>>>(transformations | std::ranges::views::transform(transformationValues));

    EXPECT_THAT(transformationV, ElementsAre(
        std::vector<long>{1, 0, 0, 0, 1, 0},
        std::vector<long>{1, 0, -20, 0, 1, 0},
        std::vector<long>{1, 0, 20, 0, 1, 0},
        std::vector<long>{1, 0, 189, 0, 1, -159},
        std::vector<long>{1, 0, -158, 0, 1, 189}
    ));

    const auto commonPart = aligner.imagesCommonPart(photos, transformations);
    EXPECT_EQ(commonPart, QRect(158, 159, 1633, 1652));
}
