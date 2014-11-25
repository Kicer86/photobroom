
#include <QString>
#include <QImage>
#include <QDebug>
#include <QImageWriter>

int main(int argc, char** argv)
{
    if (argc < 3)
        return 1;

    const QString src = argv[1];
    const QString dst = argv[2];

    QImage srcImg(src);

    qCritical() << "dupa" << src << dst;

    QImageWriter dstImg(dst);
    const bool status = dstImg.write(srcImg);

    if(!status)
        qCritical() << dstImg.errorString();

    return status? 0: 2;
}
