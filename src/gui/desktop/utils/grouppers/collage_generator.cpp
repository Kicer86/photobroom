
#include <QPainter>
#include <core/oriented_image.hpp>

#include "collage_generator.hpp"

// materials: https://stackoverflow.com/questions/8535633/photo-collage-algorithm

namespace
{
    bool isHorizontal(const QImage& image)
    {
        return image.width() > image.height();
    }
}


CollageGenerator::CollageGenerator(IExifReader& exifReader)
    : m_exifReader(exifReader)
{

}


QImage CollageGenerator::generateCollage(const QStringList& paths) const
{
    QStringList toUse;

    const auto count = paths.size();

    if (count <= 4)
        toUse = paths;
    else
    {
        const int step = std::round(count / 4.0);

        toUse.push_back(paths[0]);
        toUse.push_back(paths[step]);
        toUse.push_back(paths[count - 1 - step]);
        toUse.push_back(paths[count - 1]);
    }

    QList<QImage> images;
    std::transform(toUse.begin(), toUse.end(), std::back_inserter(images), [this](const auto& path)
    {
        return OrientedImage(m_exifReader, path).get();
    });

    //std::partition(images.begin(), images.end(), isHorizontal);

    const QImage collage = merge(images.begin(), images.end());

    return collage;
}


QImage CollageGenerator::merge(QList<QImage>::iterator first, QList<QImage>::iterator last) const
{
    QImage result;

    if (last - first == 2)
        result = merge(*first, *(first + 1));
    else
    {
        QImage partial = merge(first + 1, last);
        result = merge(partial, *first);
    }

    return result;
}


QImage CollageGenerator::merge(const QImage& lhs, const QImage& rhs) const
{
    QImage result;

    QImage leftImage  = lhs;
    QImage rightImage = rhs;
    const bool isLeftHorizontal  = isHorizontal(leftImage);
    const bool isRightHorizontal = isHorizontal(rightImage);

    // merge one over another
    if (isLeftHorizontal && isRightHorizontal)
    {
        adjustHorizontalSizes(leftImage, rightImage);
        assert(leftImage.width() == rightImage.width());

        QImage image(QSize(leftImage.width(), leftImage.height() + rightImage.height()), QImage::Format_ARGB32);
        image.fill(Qt::white);

        QPainter painter(&image);
        painter.drawImage(0, 0, leftImage);
        painter.drawImage(0, leftImage.height(), rightImage);

        result = image;
    }
    else
    {
        adjustVerticalSizes(leftImage, rightImage);
        assert(leftImage.height() == rightImage.height());

        QImage image(QSize(leftImage.width() + rightImage.width(), leftImage.height()), QImage::Format_ARGB32);
        image.fill(Qt::white);

        QPainter painter(&image);
        painter.drawImage(0, 0, leftImage);
        painter.drawImage(leftImage.width(), 0, rightImage);

        result = image;
    }

    return result;
}


void CollageGenerator::adjustHorizontalSizes(QImage& lhs, QImage& rhs) const
{
    if (lhs.width() > rhs.width())
        lhs = lhs.scaledToWidth(rhs.width(), Qt::SmoothTransformation);
    else if (rhs.width() > lhs.width())
        rhs = rhs.scaledToWidth(lhs.width(), Qt::SmoothTransformation);
}


void CollageGenerator::adjustVerticalSizes(QImage& lhs, QImage& rhs) const
{
    if (lhs.height() > rhs.height())
        lhs = lhs.scaledToHeight(rhs.height(), Qt::SmoothTransformation);
    else if (rhs.height() > lhs.height())
        rhs = rhs.scaledToHeight(lhs.height(), Qt::SmoothTransformation);
}
