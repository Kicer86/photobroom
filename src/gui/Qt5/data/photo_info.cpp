
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
    m_multpilexer(),
    m_photoData(),
    m_thumbnailData()
{
    m_multpilexer.setGetter(getter);
    load();
}


PhotoInfo::~PhotoInfo()
{
    delete m_thumbnail;
    delete m_thumbnailRaw;
}


const RawPhotoData& PhotoInfo::rawPhotoData()
{
    const QPixmap photo = getPhoto();
    QImage image = photo.toImage();

    if (m_photoData.data == nullptr)
    {
        m_photoData.size = image.byteCount();
        m_photoData.data = new uchar[m_photoData.size];

        memcpy(m_photoData.data, image.bits(), m_photoData.size);
    }

    return m_photoData;
}


const RawPhotoData& PhotoInfo::rawThumbnailData()
{
    if (m_thumbnailRaw == nullptr)
    {
        m_thumbnailRaw = new QImage;
        *m_thumbnailRaw = m_thumbnail->toImage();
    }

    m_thumbnailData.size = m_thumbnailRaw->byteCount();
    m_thumbnailData.data = m_thumbnailRaw->bits();

    return m_thumbnailData;
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
