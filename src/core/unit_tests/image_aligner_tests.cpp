
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


MATCHER_P(isSimilarTo, v, "")
{
    using argT = std::remove_cvref_t<decltype(arg)>;
    using vT = std::remove_cvref_t<decltype(v)>;
    static_assert(std::is_same_v<argT, vT>, "Argument and expected value are of different types");

    if constexpr (is_std_vector<argT>::value)
    {
        const auto s = arg.size();
        assert(v.size() == s);

        for(std::size_t i = 0; i < s; i++)
            if (areNotSimilar(arg[i], v[i]))
                return false;
    }
    else if constexpr (std::is_same_v<argT, QRect>)
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
        static_assert(always_false<argT>::value, "Argument type is not supported");

    return true;
}


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

    ImageAligner aligner(photos);

    const auto transformations = aligner.calculateTransformations();

    auto transformationV = range_to<std::vector<std::vector<long>>>(transformations | std::ranges::views::transform(transformationValues));

    // allow 1 pixel differences as different versions of openCV produce a little bit different results
    EXPECT_THAT(transformationV, ElementsAre(
        isSimilarTo(std::vector<long>{1, 0, 0, 0, 1, 0}),
        isSimilarTo(std::vector<long>{1, 0, -20, 0, 1, 0}),
        isSimilarTo(std::vector<long>{1, 0, 20, 0, 1, 0}),
        isSimilarTo(std::vector<long>{1, 0, 189, 0, 1, -159}),
        isSimilarTo(std::vector<long>{1, 0, -158, 0, 1, 189})
    ));

    const auto commonPart = aligner.imagesCommonPart(transformations);
    EXPECT_THAT(commonPart, isSimilarTo(QRect(158, 159, 1633, 1652)));
}
