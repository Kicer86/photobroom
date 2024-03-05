
#include <ranges>
#include <opencv2/opencv.hpp>

#include <core/containers_utils.hpp>

#include "hdr_assembler.hpp"


// based on https://gregorkovalcik.github.io/opencv_contrib/tutorial_hdr_imaging.html
// and https://docs.opencv.org/4.9.0/d2/df0/tutorial_py_hdr.html


namespace HDR
{
    void assemble(const QStringList& photoPaths, const QString& outputFile)
    {
        const auto images = photoPaths | std::views::transform([](const QString& path){ return cv::imread(path.toStdString()); });

        const cv::Mat hdr = assemble(range_to<std::vector<cv::Mat>>(images));
        cv::imwrite(outputFile.toStdString(), hdr);
    }


    cv::Mat assemble(const std::vector<cv::Mat>& photos)
    {
        cv::Mat mertenes_hdr;
        const auto mertenes = cv::createMergeMertens();
        mertenes->process(photos, mertenes_hdr);

        const cv::Mat mertenes_hdr_8bit = mertenes_hdr * 255;
        return mertenes_hdr_8bit;
    }
}
