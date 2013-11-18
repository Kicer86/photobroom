
#include "photo_info.hpp"

#include <memory>

#include <QString>
#include <QPixmap>

#include "core/types.hpp"
#include "core/tag_feeder.hpp"

namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}


struct PhotoInfo::Data
{
    Data(const QString &p): pixmap(), path(p), tags()
    {
        std::unique_ptr<ITagData> p_tags = TagFeeder::getTagsFor(p.toStdString());

        tags = std::move(p_tags);
    }
    
    Data(const Data& other): 
        pixmap(other.pixmap),
        path(other.path),
        tags(other.tags)
    {}
    
    QPixmap pixmap;
    QString path;
    std::shared_ptr<ITagData> tags;
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


std::shared_ptr<ITagData> PhotoInfo::getTags() const
{
    return m_data->tags;
}
