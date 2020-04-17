
#ifndef PHOTO_PROPERTIES_HPP
#define PHOTO_PROPERTIES_HPP

#include <QObject>
#include <QRect>

struct PhotoProperties
{
    Q_PROPERTY(QString path MEMBER m_path)
    Q_PROPERTY(QSize size MEMBER m_size)

    PhotoProperties();
    PhotoProperties(const PhotoProperties &) = default;
    PhotoProperties(const QString& path, const QSize &);

    QString m_path;
    QSize m_size;

    Q_GADGET
};

#endif
