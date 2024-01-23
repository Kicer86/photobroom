
#include <opencv2/opencv.hpp>

#include "image_aligner.hpp"


bool ImageAligner::align(const QString& outputDir, const QString& prefix, const QStringList& photos)
{
    if (photos.size() < 2)
        return false;

    const cv::Size winSize(31, 31);
    const cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);

    const auto& first = photos.front();
    const auto firstImg = cv::imread(first.toStdString());

    cv::Mat firstGray;
    cv::cvtColor(firstImg, firstGray, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> firstCorners;
    cv::goodFeaturesToTrack(firstGray, firstCorners, 100, 0.01, 10);

    cv::imwrite(QString("%1/%2.jpg").arg(outputDir).arg(0).toStdString(), firstGray);

    for (int i = 1; i < photos.size(); i++)
    {
        const auto& next = photos[i];
        const auto nextImg = cv::imread(next.toStdString());

        cv::Mat nextGray;
        cv::cvtColor(nextImg, nextGray, cv::COLOR_RGB2GRAY);

        std::vector<cv::Point2f> nextCorners;
        std::vector<uchar> status;
        std::vector<float> err;
        cv::calcOpticalFlowPyrLK(firstGray, nextGray, firstCorners, nextCorners, status, err, winSize, 3, termcrit, 0, 0.001);

        const cv::Mat transform = cv::estimateAffinePartial2D(firstCorners, nextCorners, cv::noArray(), cv::RANSAC);

        cv::Mat nextStabilized;
        cv::warpAffine(nextGray, nextStabilized, transform, nextGray.size());

        cv::imwrite(QString("%1/%2.jpg").arg(outputDir).arg(i).toStdString(), nextStabilized);
    }

    return true;
}
