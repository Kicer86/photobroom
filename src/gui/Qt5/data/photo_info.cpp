
#include "photo_info.hpp"

#include <assert.h>

#include <QPixmap>
#include <QImage>

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


PhotoInfo::PhotoInfo(const std::string& path, IThreadMultiplexer< PhotoInfo * >::IGetter* getter):
    APhotoInfo(path),
    m_thumbnail(new QPixmap),
    m_thumbnailRaw(nullptr),
    m_multpilexer()
{
    m_multpilexer.setGetter(getter);
    load();
}


PhotoInfo::~PhotoInfo()
{
    delete m_thumbnail;
    delete m_thumbnailRaw;
}


RawPhotoData PhotoInfo::rawPhotoData()
{
    //TODO: introduce some cache for image/raw data
    const QPixmap photo = getPhoto();
    QImage image = photo.toImage();

    RawPhotoData data;

    data.size = image.byteCount();
    data.data = new uchar[data.size];

    memcpy(data.data, image.bits(), data.size);

    return data;
}


RawPhotoData PhotoInfo::rawThumbnailData()
{
    RawPhotoData data;

    if (m_thumbnailRaw == nullptr)
    {
        m_thumbnailRaw = new QImage;
        *m_thumbnailRaw = m_thumbnail->toImage();
    }

    data.data = m_thumbnailRaw->bits();
    data.size = m_thumbnailRaw->byteCount();

    return data;
}


const QPixmap PhotoInfo::getPhoto() const
{
    const std::string path = APhotoInfo::getPath();
    const QPixmap pixmap(path.c_str());

    return pixmap;
}


const QPixmap &PhotoInfo::getThumbnail() const
{
    return *m_thumbnail;
}


void PhotoInfo::load()
{
    auto task = std::make_shared<ThumbnailGenerator>(this);  //generate thumbnail
    TaskExecutorConstructor::get()->add(task);

    m_thumbnail->load(":/gui/images/clock64.png");                        //use temporary thumbnail until final one is ready
}


void PhotoInfo::thumbnailReady(const QPixmap& thumbnail)
{
    *m_thumbnail = thumbnail;

    m_multpilexer.send(this);
}
