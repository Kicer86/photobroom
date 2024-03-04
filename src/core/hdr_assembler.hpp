
#ifndef HDR_ASSEMBLER_HPP_INCLUDED
#define HDR_ASSEMBLER_HPP_INCLUDED

#include <QStringList>
#include <opencv2/opencv.hpp>

#include <core_export.h>


namespace HDR
{
    CORE_EXPORT void assemble(const QStringList& photos, const QString& outputFile);
    CORE_EXPORT cv::Mat assemble(const std::vector<cv::Mat>& photos);
}

#endif
