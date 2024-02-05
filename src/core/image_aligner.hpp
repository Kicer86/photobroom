
#ifndef IMAGE_ALIGNER_HPP_INCLUDED
#define IMAGE_ALIGNER_HPP_INCLUDED

#include <QStringList>
#include <QRect>
#include <opencv2/opencv.hpp>

#include "core_export.h"


class CORE_EXPORT ImageAligner
{
public:
    ImageAligner(const QStringList& photos);

    bool align(const QString& outputDir, const QString& prefix);

    std::vector<cv::Mat> calculateTransformations();
    QRect imagesCommonPart(const std::vector<cv::Mat>& transformations);
    void applyTransformations(const std::vector<cv::Mat>& transformations, const QString& outputDir);

private:
    const QStringList m_photos;
};

#endif
