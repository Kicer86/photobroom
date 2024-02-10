
#ifndef IMAGE_ALIGNER_HPP_INCLUDED
#define IMAGE_ALIGNER_HPP_INCLUDED

#include <QStringList>
#include <QRect>
#include <opencv2/opencv.hpp>

#include "core_export.h"


class CORE_EXPORT AlignedImages final
{
public:
    AlignedImages(const QStringList& photos, const QRect& imageSize, const std::vector<cv::Mat>& transformations);

    void forEachImage(std::function<void(const cv::Mat &)>) const;

    const std::vector<cv::Mat>& transformations() const;
    QRect imagesCommonPart() const;

private:
    const std::vector<cv::Mat> m_transformations;
    const QRect m_commonPart;
    const QStringList m_photos;
};


class CORE_EXPORT ImageAligner final
{
public:
    explicit ImageAligner(const QStringList& photos);

    std::optional<AlignedImages> align() const;

private:
    std::vector<cv::Mat> calculateTransformations() const;
    QRect imagesCommonPart(const std::vector<cv::Mat>& transformations) const;

    const QStringList m_photos;
};

#endif
