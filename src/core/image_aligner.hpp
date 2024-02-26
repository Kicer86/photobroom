
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
    virtual ~IAlignedImages() = default;

    /** @brief Align each image and pass result to provided function.
     */
    virtual void forEachImage(std::function<void(const cv::Mat &)>) const = 0;

    /** @brief return transformations which will be applied on photos when @ref forEachImage is called.
     */
    virtual const std::vector<cv::Mat>& transformations() const = 0;

    /**
     * @brief Return crop
     */
    virtual QRect imagesCommonPart() const = 0;
};

/**
 * @brief ImageAligner builds all data needed to align given images and produces @ref AlignedImages
 */
class CORE_EXPORT ImageAligner final
{
public:
    explicit ImageAligner(const QStringList& photos, const ILogger &);
    ~ImageAligner();

    ImageAligner& registerProgress(std::function<void(int, int)>);

    /** @brief create object providing access to aligned photos.
     *         After this method is called, ImageAligner becomes invalid and should not be used.
     */
    std::unique_ptr<IAlignedImages> align();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif
