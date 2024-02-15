
#ifndef IMAGE_ALIGNER_HPP_INCLUDED
#define IMAGE_ALIGNER_HPP_INCLUDED

#include <QStringList>
#include <QRect>
#include <opencv2/opencv.hpp>

#include "core_export.h"
#include <core/ilogger.hpp>

/**
 * @brief IAlignedImages is an interface returned by @ref ImageAligner providing access to aligned images.
 */
class IAlignedImages
{
public:
    virtual void forEachImage(std::function<void(const cv::Mat &)>) const = 0;

    virtual const std::vector<cv::Mat>& transformations() const = 0;
    virtual QRect imagesCommonPart() const = 0;
};

/**
 * @brief ImageAligner builds all data needed to align given images and produces @ref AlignedImages
 */
class CORE_EXPORT ImageAligner final: IAlignedImages
{
public:
    explicit ImageAligner(const QStringList& photos, const ILogger &);

    const IAlignedImages* align();

private:
    void forEachImage(std::function<void(const cv::Mat &)>) const override;

    const std::vector<cv::Mat>& transformations() const override;
    QRect imagesCommonPart() const override;

    std::vector<cv::Mat> calculateTransformations() const;

    const QStringList m_photos;
    std::unique_ptr<ILogger> m_logger;

    std::vector<cv::Mat> m_transformations;
    QRect m_commonPart;
};

#endif
