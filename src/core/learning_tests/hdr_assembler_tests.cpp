
#include <gtest/gtest.h>

#include <unit_tests_utils/empty_logger.hpp>
#include <core/hdr_assembler.hpp>
#include <core/image_aligner.hpp>

// Images for tests:
//   https://en.wikipedia.org/wiki/Multi-exposure_HDR_capture
//   https://www.easyhdr.com/examples/wadi-rum-sunset/


TEST(HDRAssemblerTest, desert)
{
    const QStringList photos = {"wadi-rum-sunset1.jpg", "wadi-rum-sunset2.jpg", "wadi-rum-sunset3.jpg"};

    HDR::assemble(photos, "desert.jpg");
}


TEST(HDRAssemblerTest, city)
{
    EmptyLogger logger;

    const QStringList photos = {"StLouisArchMultExpEV-4.72.JPG", "StLouisArchMultExpEV-1.82.JPG", "StLouisArchMultExpEV+1.51.JPG", "StLouisArchMultExpEV+4.09.JPG"};

    auto alignedImages = ImageAligner(photos, logger).align();

    std::vector<cv::Mat> alignedMats;
    alignedImages->forEachImage([&alignedMats](const cv::Mat& mat) { alignedMats.push_back(mat); } );

    const auto hdrMat = HDR::assemble(alignedMats);

    cv::imwrite("city.jpg", hdrMat);
}
