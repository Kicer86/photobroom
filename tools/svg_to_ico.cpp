#include <QDebug>
#include <QGuiApplication>
#include <QImage>
#include <QImageWriter>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>

int main(int argc, char** argv)
{
    const QGuiApplication application(argc, argv);

    if (argc != 5)
    {
        qCritical() << "Usage: svg_to_ico <input.svg> <output.ico> <width> <height>";
        return 1;
    }

    bool widthValid = false;
    bool heightValid = false;
    const auto width = QString::fromLocal8Bit(argv[3]).toInt(&widthValid);
    const auto height = QString::fromLocal8Bit(argv[4]).toInt(&heightValid);

    if (!widthValid || !heightValid || width <= 0 || height <= 0)
    {
        qCritical() << "Width and height must be positive integers";
        return 1;
    }

    QSvgRenderer renderer(QString::fromLocal8Bit(argv[1]));
    if (!renderer.isValid())
    {
        qCritical() << "Could not read SVG file:" << argv[1];
        return 2;
    }

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();

    QImageWriter writer(QString::fromLocal8Bit(argv[2]), "ico");
    if (!writer.write(image))
    {
        qCritical() << "Could not write ICO file:" << writer.errorString();
        return 3;
    }

    return 0;
}
