
#include <gmock/gmock.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/img_hash.hpp>
#include <QImage>


const std::string ImagesPath(IMAGES_PATH);

TEST(PHashTest, QImageToMatConversion)
{
    // construct algorithm
    const auto hashAlgorithm = cv::img_hash::PHash::create();

    // load image using cv methods and calculate hash
    const cv::Mat cvmat = cv::imread(ImagesPath + "/img1_1000.png");
    cv::Mat cvmat_output;

    cv::img_hash::pHash(cvmat, cvmat_output);

    // load image using qt methods, convert it to compatible cv format and calculate hash
    QImage qimage((ImagesPath + "/img1_1000.png").c_str());

    ASSERT_EQ(qimage.format(), QImage::Format_RGB32);

    const cv::Mat cvArgbImage(
        qimage.height(),
        qimage.width(),
        CV_8UC4,
        qimage.bits(),
        static_cast<std::size_t>(qimage.bytesPerLine())
    );

    cv::Mat argbImage_output;
    hashAlgorithm->compute(cvArgbImage, argbImage_output);

    // test same outputs
    const auto comparisonResultForSame = hashAlgorithm->compare(cvmat_output, cvmat_output);
    EXPECT_DOUBLE_EQ(comparisonResultForSame, 0.0);

    // results should be the same for Qt vs cv::Mat
    const auto comparisonResult = hashAlgorithm->compare(cvmat_output, argbImage_output);
    EXPECT_DOUBLE_EQ(comparisonResult, 0.0);
}
