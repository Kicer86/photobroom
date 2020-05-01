
#ifndef PHOTO_PROPERTIES_HPP
#define PHOTO_PROPERTIES_HPP

#include <QObject>
#include <QRect>

#include <database/photo_types.hpp>


struct PhotoProperties
{
    Q_PROPERTY(QString path MEMBER m_path)
    Q_PROPERTY(QSize size MEMBER m_size)
    Q_PROPERTY(Photo::Id id MEMBER m_id)

    PhotoProperties();
    PhotoProperties(const PhotoProperties &) = default;
    PhotoProperties(const QString& path, const QSize &, const Photo::Id &);

    QString m_path;
    QSize m_size;
    Photo::Id m_id;

    Q_GADGET
};

#endif
