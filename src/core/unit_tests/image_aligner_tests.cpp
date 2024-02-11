
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ranges>

#include "containers_utils.hpp"
#include "image_aligner.hpp"

using testing::ElementsAre;


template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};


template<typename T> struct always_false : std::false_type {};


template<typename T>
bool areNotSimilar(const T& lhs, const T& rhs)
{
    return std::abs(lhs - rhs) > 1;
}


template<typename T>
bool isSimilarToImpl(const T& arg, const T& v)
{
    if constexpr (is_std_vector<T>::value)
    {
        const auto s = arg.size();
        assert(v.size() == s);

        using vector_value_type = T::value_type;

        if constexpr (is_std_vector<vector_value_type>::value)            // vector in vector
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
        static_assert(always_false<T>::value, "Argument type is not supported");

    return true;
}


MATCHER_P(isSimilarTo, v, "")
{
    using argT = std::remove_cvref_t<decltype(arg)>;
    using vT = std::remove_cvref_t<decltype(v)>;
    static_assert(std::is_same_v<argT, vT>, "Argument and expected value are of different types");

    return isSimilarToImpl(arg, v);
}


namespace
{
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
    constexpr bool strictMode = true;
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
            QRect(158, 159, 1631, 1651)
        },
        TestParams
        {
            {
                "alterd_images/img1_.png",
                "alterd_images/img1_.png",
                "alterd_images/img1_.png",
            },
            true,
            {
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
                std::vector<long>{1, 0, 0,  0, 1, 0,  0, 0, 1},
            },
            QRect(0, 0, 1999, 1999)
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
                std::vector<long>{1, 0, 0,    0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, -20,  0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, 20,   0, 1, 0,     0, 0, 1},
                std::vector<long>{1, 0, 112,  0, 1, -139,  0, 0, 1},
                std::vector<long>{1, 0, -86,  0, 1, 157,   0, 0, 1},
            },
            QRect(86, 140, 1488, 840)
        }
    )
);


TEST_P(ImageAlignerTest, validImages)
{
    const auto& [photos, exact, expectedTransformations, crop] = GetParam();

    const ImageAligner aligner(photos);
    const auto alignedImages = aligner.align();
    ASSERT_TRUE(alignedImages.has_value());

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

    // check if all images have size of cropped area
    const auto imgSize = commonPart.size();
    alignedImages->forEachImage([imgSize](const auto image)
    {
        EXPECT_EQ(image.size().width, imgSize.width());
        EXPECT_EQ(image.size().height, imgSize.height());
    });
}
