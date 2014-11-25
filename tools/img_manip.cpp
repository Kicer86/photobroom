
#include <QString>
#include <QImage>

int main(int argc, char* argv[])
{
    if (argc < 2)
        return 1;

    const QString src = argv[1];
    const QString dst = argv[2];

    QImage srcImg(src);

    const bool status = srcImg.save(dst);

    return status? 0: 2;
}
