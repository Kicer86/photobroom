
#include "photo_info.hpp"

#include <QString>
#include <QPixmap>

#include "core/types.hpp"

namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}


struct PhotoInfo::Data
{
    Data(const QString &p): pixmap(), path(p), tags() {}
    
    Data(const Data& other): 
        pixmap(other.pixmap),
        path(other.path),
        tags(other.tags)
    {}
    
    QPixmap pixmap;
    QString path;
    TagData tags;
};


//TODO: scaling in thread, temporary bitmap

PhotoInfo::PhotoInfo(const QString &p): m_data(new Data(p))
{
    QPixmap tmp(p);

    m_data->pixmap = tmp.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


PhotoInfo::PhotoInfo(const PhotoInfo& other): m_data( new Data(*other.m_data.get()) )
{

}


PhotoInfo::~PhotoInfo()
{

}


const QString& PhotoInfo::getPath() const
{
    return m_data->path;
}


const QPixmap& PhotoInfo::getPixmap() const
{
    return m_data->pixmap;
}


ITagData* PhotoInfo::getTags() const
{
    return &m_data->tags;
}
