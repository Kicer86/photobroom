#include <QDebug>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QImageWriter>
#include <QPainter>
#include <QSize>
#include <QString>
#include <QSvgRenderer>

int main(int argc, char** argv)
{
    const QGuiApplication application(argc, argv);

    if (argc != 6)
    {
        qCritical() << "Usage: svg_to_image <input.svg> <output> <width|-1> <height|-1> <white|transparent>";
        return 1;
    }

    bool widthValid = false;
    bool heightValid = false;
    const auto width = QString::fromLocal8Bit(argv[3]).toInt(&widthValid);
    const auto height = QString::fromLocal8Bit(argv[4]).toInt(&heightValid);

    const bool useDefaultSize = width == -1 && height == -1;
    const bool useExplicitSize = width > 0 && height > 0;
    if (!widthValid || !heightValid || (!useDefaultSize && !useExplicitSize))
    {
        qCritical() << "Width and height must both be positive integers or both be -1";
        return 1;
    }

    const auto background = QString::fromLocal8Bit(argv[5]);
    if (background != QStringLiteral("white") && background != QStringLiteral("transparent"))
    {
        qCritical() << "Background must be either 'white' or 'transparent'";
        return 1;
    }

    QSvgRenderer renderer(QString::fromLocal8Bit(argv[1]));
    if (!renderer.isValid())
    {
        qCritical() << "Could not read SVG file:" << argv[1];
        return 2;
    }

    const QSize imageSize = useDefaultSize ? renderer.defaultSize() : QSize(width, height);
    if (!imageSize.isValid())
    {
        qCritical() << "SVG file does not define a valid default size:" << argv[1];
        return 2;
    }

    const bool transparent = background == QStringLiteral("transparent");
    QImage image(imageSize, transparent ? QImage::Format_ARGB32 : QImage::Format_RGB32);
    image.fill(transparent ? Qt::transparent : Qt::white);

    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();

    const auto outputPath = QString::fromLocal8Bit(argv[2]);
    auto outputFormat = QFileInfo(outputPath).suffix().toLatin1().toLower();
    if (outputFormat == "jpg")
        outputFormat = "jpeg";

    QImageWriter writer(outputPath, outputFormat);
    if (!writer.write(image))
    {
        qCritical() << "Could not write image file:" << writer.errorString();
        return 3;
    }

    return 0;
}
