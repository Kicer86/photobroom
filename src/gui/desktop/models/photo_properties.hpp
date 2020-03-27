
#ifndef PHOTO_PROPERTIES_HPP
#define PHOTO_PROPERTIES_HPP

#include <QObject>

struct PhotoProperties
{
    Q_PROPERTY(QString path MEMBER m_path)
    Q_PROPERTY(int height MEMBER m_height)
    Q_PROPERTY(int width MEMBER m_width)

    PhotoProperties();
    PhotoProperties(const PhotoProperties &) = default;
    PhotoProperties(const QString& path, int h, int w);

    QString m_path;
    int m_height;
    int m_width;

    Q_GADGET
};

#endif
