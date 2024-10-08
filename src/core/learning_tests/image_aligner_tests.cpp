
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ranges>
#include <QDir>

#include <core/generic_concepts.hpp>
#include <unit_tests_utils/empty_logger.hpp>
#include <unit_tests_utils/printers.hpp>
#include "containers_utils.hpp"
#include "image_aligner.hpp"


using testing::ElementsAre;

namespace
{
    template<typename T>
    bool areNotSimilar(const T& lhs, const T& rhs)
    {
        return std::abs(lhs - rhs) > 1;
    }


    template<typename T>
    bool isSimilarToImpl(const T& arg, const T& v)
    {
        if constexpr (is_std_vector_v<T>)
        {
            const auto s = arg.size();
            assert(v.size() == s);

            using vector_value_type = T::value_type;

            if constexpr (is_std_vector_v<vector_value_type>)               // vector in vector
            {
                for(std::size_t i = 0; i < s; i++)
                    if (isSimilarToImpl(arg[i], v[i]) == false)
                        return false;
            }
            else
            {
                for(std::size_t i = 0; i < s; i++)
                    if (areNotSimilar(arg[i], v[i]))
                        return false;
            }
        }
        else if constexpr (std::is_same_v<T, QRect>)
        {
            if (areNotSimilar(arg.x(), v.x()))
                return false;
            if (areNotSimilar(arg.y(), v.y()))
                return false;
            if (areNotSimilar(arg.width(), v.width()))
                return false;
            if (areNotSimilar(arg.height(), v.height()))
                return false;
        }
        else
            static_assert(always_false_v<T>, "Argument type is not supported");

        return true;
    }

    std::vector<long> transformationValues(const cv::Mat& mat)
    {
        assert(mat.size().width == 3);
        assert(mat.size().height == 3);

        return {
            std::lround(mat.at<float>(0, 0)),
            std::lround(mat.at<float>(0, 1)),
            std::lround(mat.at<float>(0, 2)),

            std::lround(mat.at<float>(1, 0)),
            std::lround(mat.at<float>(1, 1)),
            std::lround(mat.at<float>(1, 2)),

            std::lround(mat.at<float>(2, 0)),
            std::lround(mat.at<float>(2, 1)),
            std::lround(mat.at<float>(2, 2))
        };
    }

    // some tests fail depending on openCV version.
    // Setting this flag to true will cause them to fall.
    // Setting it to false, loosens conditions and lets them pass
    constexpr bool strictMode = false;
}


MATCHER_P(isSimilarTo, v, "")
{
    using argT = std::remove_cvref_t<decltype(arg)>;
    using vT = std::remove_cvref_t<decltype(v)>;
    static_assert(std::is_same_v<argT, vT>, "Argument and expected value are of different types");

    return isSimilarToImpl(arg, v);
}


using TestParams = std::tuple<QStringList, bool, std::vector<std::vector<long>>, QRect>;


class ImageAlignerTest: public testing::TestWithParam<TestParams> {};


INSTANTIATE_TEST_SUITE_P
(
    validImages,
    ImageAlignerTest,
    testing::Values
    (
        TestParams
        {
            {
                "alterd_images/img1_.png",
                "alterd_images/img1_moved_left.png",
                "alterd_images/img1_moved_right.png",
                "alterd_images/img1_rotated_negative.png",
                "alterd_images/img1_rotated.png",
            },
            strictMode,
            {
                std::vector<long>{1, 0, 0,     0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, -20,   0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, 21,    0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, 189,   0, 1, -158,  0, 0, 1},
                std::vector<long>{1, 0, -158,  0, 1, 189,   0, 0, 1},
            },
            QRect(159, 159, 1652, 1652)
        },
        TestParams
        {
            {
                "alterd_images/img1_.png",
                "alterd_images/img1_.png",
                "alterd_images/img1_.png",
            },
            strictMode,
            {
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
            },
            QRect(1, 1, 1999, 1999)
        },
        TestParams
        {
            {
                "alterd_images/img2_.png",
                "alterd_images/img2_moved_left.png",
                "alterd_images/img2_moved_right.png",
                "alterd_images/img2_rotated_negative.png",
                "alterd_images/img2_rotated.png",
            },
            strictMode,
            {
                std::vector<long>{1, 0, 0,    0, 1, 0,    0, 0, 1},
                std::vector<long>{1, 0, -20,  0, 1, 0,    0, 0, 1},
                std::vector<long>{1, 0, 20,   0, 1, 0,    0, 0, 1},
                std::vector<long>{1, 0, 26,   0, 1, 23,   0, 0, 1},
                std::vector<long>{1, 0, -28,  0, 1, -22,  0, 0, 1},
            },
            QRect(28, 22, 643, 793)
        }
    )
);


TEST_P(ImageAlignerTest, validImages)
{
    EmptyLogger logger;

    const auto& [photos, exact, expectedTransformations, crop] = GetParam();
    ImageAligner aligner(photos, logger);
    const auto alignedImages = aligner.align();
    ASSERT_TRUE(alignedImages);

    const auto transformations = alignedImages->transformations();

    auto transformationsV = range_to<std::vector<std::vector<long>>>(transformations | std::ranges::views::transform(transformationValues));

    // verify transformations to be applied for each image
    // if 'exact' is false then allow 1 pixel differences (different versions of openCV produce a little bit different results)
    if (exact)
        EXPECT_EQ(transformationsV, expectedTransformations);
    else
        EXPECT_THAT(transformationsV, isSimilarTo(expectedTransformations));

    // verify part of first image to which all images will be cropped
    const auto commonPart = alignedImages->imagesCommonPart();
    if (exact)
        EXPECT_EQ(commonPart, crop);
    else
        EXPECT_THAT(commonPart, isSimilarTo(crop));

    static int i = 0;
    int j = 0;
    QDir().mkdir("test_results");

    // check if all images have size of cropped area
    const auto imgSize = commonPart.size();
    alignedImages->forEachImage([&](const auto image)
    {
        EXPECT_EQ(image.size().width, imgSize.width());
        EXPECT_EQ(image.size().height, imgSize.height());

        cv::imwrite("test_results/" + std::to_string(i) + "_" + std::to_string(j++) + ".png", image);
    });

    i++;
}
