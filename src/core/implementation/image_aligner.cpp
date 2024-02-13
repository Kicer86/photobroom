
#include <QDir>

#include "image_aligner.hpp"

// Based on: https://stackoverflow.com/questions/62495112/aligning-and-cropping-same-scene-images


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


AlignedImages::AlignedImages(const QStringList& photos, const QRect& imageSize, const std::vector<cv::Mat>& transformations)
    : m_transformations(transformations)
    , m_commonPart(commonPart(imageSize, transformations))
    , m_photos(photos)
{
}


void AlignedImages::forEachImage(std::function<void(const cv::Mat &)> op) const
{
    // adjust images
    for (int i = 0; i < m_photos.size(); i++)
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


const std::vector<cv::Mat>& AlignedImages::transformations() const
{
    return m_transformations;
}



QRect AlignedImages::imagesCommonPart() const
{
    return m_commonPart;
}



ImageAligner::ImageAligner(const QStringList& photos, const ILogger& logger)
    : m_photos(photos)
    , m_logger(logger.subLogger("ImageAligner"))
{

}


std::optional<AlignedImages> ImageAligner::align() const
{
    const auto transformations = calculateTransformations();
    if (transformations.empty())
        return {};

    const auto& first = m_photos.front();
    const auto referenceImage = cv::imread(first.toStdString());
    QRect firstImageSize(0, 0, referenceImage.size().width, referenceImage.size().height);

    return AlignedImages(m_photos, firstImageSize, transformations);
}


std::vector<cv::Mat> ImageAligner::calculateTransformations() const
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

    for (int i = 1; i < m_photos.size(); i++)
    {
        const auto& next = m_photos[i];
        const auto image = cv::imread(next.toStdString());

        cv::Mat imageGray;
        cv::cvtColor(image, imageGray, cv::COLOR_RGB2GRAY);

        const auto transformation = findTransformation(referenceImageGray, imageGray);
        m_logger->trace(QString("Transformation for photo '%1': %2").arg(next).arg(mat(transformation)));

        transformations.push_back(transformation);
    }

    return transformations;
}
