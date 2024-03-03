
#include <ranges>
#include <opencv2/opencv.hpp>

#include <core/containers_utils.hpp>

#include "hdr_assembler.hpp"


// based on https://gregorkovalcik.github.io/opencv_contrib/tutorial_hdr_imaging.html
// and https://docs.opencv.org/4.9.0/d2/df0/tutorial_py_hdr.html


namespace HDR
{
    namespace
    {
        double photoExpTime(IExifReader& exif, const QString& path)
        {
            const auto shutterSpeed = exif.get(path, IExifReader::TagType::ShutterSpeed);
            const auto expTime = std::any_cast<double>(shutterSpeed.value_or(0.0));

            return expTime;
        }
    }

    bool assemble(IExifReader& exif, const QStringList& photoPaths, const QString& outputFile)
    {
        const auto images = photoPaths | std::views::transform([](const QString& path){ return cv::imread(path.toStdString()); });
        const auto exp_time = photoPaths | std::views::transform([&exif](const QString& path) { return photoExpTime(exif, path); });

        /*
        cv::Mat debvec_hdr;
        const auto debvec = cv::createMergeDebevec();
        debvec->process(range_to<std::vector<cv::Mat>>(images), debvec_hdr, range_to<std::vector<float>>(exp_time));

        cv::Mat debvec_res;
        const auto tonemap = cv::createTonemap(2.2f);
        tonemap->process(debvec_hdr, debvec_res);

        cv::Mat debvec_res_8bit = debvec_res * 255;

        cv::imwrite(outputFile.toStdString(), debvec_res_8bit);
        */

        cv::Mat mertenes_hdr;
        const auto mertenes = cv::createMergeMertens();
        mertenes->process(range_to<std::vector<cv::Mat>>(images), mertenes_hdr);

        cv::Mat mertenes_hdr_8bit = mertenes_hdr * 255;
        cv::imwrite(outputFile.toStdString(), mertenes_hdr_8bit);

        return true;
    }
}
