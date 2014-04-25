
#include "photo_info.hpp"

#include <assert.h>

#include <QPixmap>
#include <QImage>
#include <QtSvg/QSvgRenderer>

#include "database/ifs.hpp"
#include "core/task_executor.hpp"


namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}

struct ThumbnailGenerator: ITaskExecutor::ITask
{
    ThumbnailGenerator(PhotoInfo* photoInfo): ITask(), m_photoInfo(photoInfo) {}
    virtual ~ThumbnailGenerator() {}

    ThumbnailGenerator(const ThumbnailGenerator &) = delete;
    ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

    virtual void perform()
    {
        QPixmap pixmap(m_photoInfo->getPath().c_str());
        QPixmap thumbnail = pixmap.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->thumbnailReady(thumbnail);
    }

    PhotoInfo* m_photoInfo;
};


PhotoInfo::PhotoInfo(const std::string& path):
    APhotoInfo(path),
    m_thumbnail(new QPixmap)
{
    load();
}


PhotoInfo::~PhotoInfo()
{
    delete m_thumbnail;
}


const QPixmap &PhotoInfo::getThumbnail() const
{
    return *m_thumbnail;
}


void PhotoInfo::load()
{
    auto task = std::make_shared<ThumbnailGenerator>(this);  //generate thumbnail
    TaskExecutorConstructor::get()->add(task);

    m_thumbnail->load(":/gui/images/clock.svg");             //use temporary thumbnail until final one is ready
}


void PhotoInfo::thumbnailReady(const QPixmap& thumbnail)
{
    *m_thumbnail = thumbnail;
}
