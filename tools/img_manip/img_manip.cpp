
#include <QString>
#include <QImage>
#include <QImageWriter>

int main(int argc, char** argv)
{
    if (argc < 3)
        return 1;

    const QString src = argv[1];
    const QString dst = argv[2];

    int size = 0;
    if (argc > 2)
        size = QString(argv[3]).toInt();

    QImage srcImg(src);

    if (size > 0)
        srcImg = srcImg.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QImageWriter dstImg(dst);
    const bool status = dstImg.write(srcImg);

    if (!status)
        qCritical() << dstImg.errorString();

    return status? 0: 2;
}
