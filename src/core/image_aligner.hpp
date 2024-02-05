
#ifndef IMAGE_ALIGNER_HPP_INCLUDED
#define IMAGE_ALIGNER_HPP_INCLUDED

#include <QStringList>
#include <QRect>
#include <opencv2/opencv.hpp>

#include "core_export.h"


class CORE_EXPORT ImageAligner
{
public:
    bool align(const QString& outputDir, const QString& prefix, const QStringList& photos);

    std::vector<cv::Mat> calculateTransformations(const QStringList& photos);
    QRect imagesCommonPart(const QStringList& photos, const std::vector<cv::Mat>& transformations);
    void applyTransformations(const QStringList& photos, const std::vector<cv::Mat>& transformations, const QString& outputDir);
};

#endif
