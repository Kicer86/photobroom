
#include <ranges>
#include <opencv2/opencv.hpp>

#include <core/containers_utils.hpp>

#include "hdr_assembler.hpp"


// based on https://gregorkovalcik.github.io/opencv_contrib/tutorial_hdr_imaging.html
// and https://docs.opencv.org/4.9.0/d2/df0/tutorial_py_hdr.html


namespace HDR
{
    bool assemble(const QStringList& photoPaths, const QString& outputFile)
    {
        const auto images = photoPaths | std::views::transform([](const QString& path){ return cv::imread(path.toStdString()); });

        cv::Mat mertenes_hdr;
        const auto mertenes = cv::createMergeMertens();
        mertenes->process(range_to<std::vector<cv::Mat>>(images), mertenes_hdr);

        cv::Mat mertenes_hdr_8bit = mertenes_hdr * 255;
        cv::imwrite(outputFile.toStdString(), mertenes_hdr_8bit);

        return true;
    }
}
