
#include <QDir>

#include "image_aligner.hpp"

// Based on: https://stackoverflow.com/questions/62495112/aligning-and-cropping-same-scene-images
//      and: https://learnopencv.com/image-alignment-ecc-in-opencv-c-python/


namespace
{
    cv::Rect rect(const QRect& r)
    {
        return cv::Rect(r.left(), r.top(), r.width(), r.height());
    }

    QString mat(const cv::Mat& m)
    {
        const cv::Mat row = m.reshape(0,1);

        std::ostringstream os;
        os << row;

        const std::string asStr = os.str();

        return QString::fromStdString(asStr);
    }

    auto findTransformation(const cv::Mat& referenceImageGray, const cv::Mat& imageGray)
    {
        const int number_of_iterations = 5000;
        const double termination_eps = 5e-5;
        const cv::TermCriteria criteria (cv::TermCriteria::COUNT + cv::TermCriteria::EPS, number_of_iterations, termination_eps);

        cv::Mat warp_matrix = cv::Mat::eye(3, 3, CV_32F);
        cv::findTransformECC(referenceImageGray, imageGray, warp_matrix, cv::MOTION_HOMOGRAPHY, criteria);

        return warp_matrix;
    }

    QRect commonPart(QRectF commonPart, const std::vector<cv::Mat>& transformations)
    {
        for(const auto& transformation: transformations)
        {
            if (transformation.at<float>(0, 2) < 0)
                commonPart.setX(static_cast<qreal>(transformation.at<float>(0, 2)) * -1);

            if (transformation.at<float>(0, 2) > 0)
                commonPart.setWidth(commonPart.width() - static_cast<qreal>(transformation.at<float>(0, 2)));

            if (transformation.at<float>(1, 2) < 0)
                commonPart.setY(static_cast<qreal>(transformation.at<float>(1, 2)) * -1);

            if (transformation.at<float>(1, 2) > 0)
                commonPart.setHeight(commonPart.height() - static_cast<qreal>(transformation.at<float>(1, 2)));
        }

        commonPart.setX(std::ceil(commonPart.x()));
        commonPart.setY(std::ceil(commonPart.y()));
        commonPart.setWidth(std::floor(commonPart.width()));
        commonPart.setHeight(std::floor(commonPart.height()));

        return commonPart.toRect();
    }
}

class ImageAligner::Impl: public IAlignedImages
{
public:
    Impl(const QStringList& photos, const ILogger& logger)
        : m_photos(photos)
        , m_logger(logger.subLogger("ImageAligner"))
    {

    }

    void forEachImage(std::function<void(const cv::Mat &)>) const override;

    const std::vector<cv::Mat>& transformations() const override;
    QRect imagesCommonPart() const override;

    std::vector<cv::Mat> calculateTransformations() const;

    const QStringList m_photos;
    std::unique_ptr<ILogger> m_logger;

    std::function<void(int, int)> m_progress;

    std::vector<cv::Mat> m_transformations;
    QRect m_commonPart;
};


ImageAligner::ImageAligner(const QStringList& photos, const ILogger& logger)
    :  m_impl(std::make_unique<Impl>(photos, logger))
{

}


ImageAligner::~ImageAligner()
{

}


ImageAligner& ImageAligner::registerProgress(std::function<void(int, int)> progress)
{
    m_impl->m_progress = progress;
    return *this;
}


std::unique_ptr<IAlignedImages> ImageAligner::align()
{
    // object has no use after producing IAlignedImages
    assert(m_impl);

    const auto transformations = m_impl->calculateTransformations();
    if (transformations.empty())
        return {};

    const auto& first = m_impl->m_photos.front();
    const auto referenceImage = cv::imread(first.toStdString());
    QRect firstImageSize(0, 0, referenceImage.size().width, referenceImage.size().height);

    m_impl->m_transformations = transformations;
    m_impl->m_commonPart = commonPart(firstImageSize, transformations);

    return std::move(m_impl);
}


std::vector<cv::Mat> ImageAligner::Impl::calculateTransformations() const
{
     if (m_photos.size() < 2)
        return {};

    const cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);

    const auto& first = m_photos.front();
    const auto referenceImage = cv::imread(first.toStdString());

    cv::Mat referenceImageGray;
    cv::cvtColor(referenceImage, referenceImageGray, cv::COLOR_RGB2GRAY);

    QRect firstImageSize(0, 0, referenceImage.size().width, referenceImage.size().height);

    // calculate required transformations
    std::vector<cv::Mat> transformations(1, []{
        cv::Mat mat = cv::Mat::eye(3, 3, CV_32F);
        mat.at<float>(0, 0) = 1.;
        mat.at<float>(1, 1) = 1.;

        return mat;
    }());  // insert empty transformation matrix for first image

    const int photos = static_cast<int>(m_photos.size());
    if (m_progress)
        m_progress(0, photos - 1);

    for (int i = 1; i < photos; i++)
    {
        const auto& next = m_photos[i];
        const auto image = cv::imread(next.toStdString());

        cv::Mat imageGray;
        cv::cvtColor(image, imageGray, cv::COLOR_RGB2GRAY);

        const auto transformation = findTransformation(referenceImageGray, imageGray);
        m_logger->trace(QString("Transformation for photo '%1': %2").arg(next).arg(mat(transformation)));

        transformations.push_back(transformation);

        if (m_progress)
            m_progress(i, photos - 1);
    }

    return transformations;
}


void ImageAligner::Impl::forEachImage(std::function<void(const cv::Mat &)> op) const
{
    const int photos = static_cast<int>(m_photos.size());

    // adjust images
    for (int i = 0; i < photos; i++)
    {
        // read image
        const auto image = cv::imread(m_photos[i].toStdString());

        // align
        cv::Mat imageAligned;
        if (i == 0)
            imageAligned = image;  // reference image does not need any transformations
        else
            cv::warpPerspective(image, imageAligned, m_transformations[i], image.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);

        // apply crop
        const auto croppedNextImg = imageAligned(rect(m_commonPart));

        // save
        op(croppedNextImg);
    }
}


const std::vector<cv::Mat>& ImageAligner::Impl::transformations() const
{
    return m_transformations;
}



QRect ImageAligner::Impl::imagesCommonPart() const
{
    return m_commonPart;
}
