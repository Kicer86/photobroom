
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QPixmap>
#include <QImage>

#include <core/task_executor.hpp>
#include <core/hash_functions.hpp>
#include <core/photos_manager.hpp>
#include <core/itagfeeder.hpp>
#include <core/tag.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/constants.hpp>
#include <database/ifs.hpp>


struct ThumbnailGenerator: ITaskExecutor::ITask
{
    ThumbnailGenerator(const IPhotoInfo::Ptr& photoInfo, int photoWidth): ITaskExecutor::ITask(), m_photoInfo(photoInfo), m_photoWidth(photoWidth) {}
    virtual ~ThumbnailGenerator() {}

    ThumbnailGenerator(const ThumbnailGenerator &) = delete;
    ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

    virtual std::string name() const override
    {
        return "Photo thumbnail generation";
    }

    virtual void perform() override
    {
        const QByteArray data = PhotosManager::instance()->getPhoto(m_photoInfo);
        QImage image;
        image.loadFromData(data);

        const QImage thumbnail = image.scaled(m_photoWidth, m_photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->initThumbnail(thumbnail);
    }

    IPhotoInfo::Ptr m_photoInfo;
    int m_photoWidth;
};


struct Sha256Assigner: public ITaskExecutor::ITask
{
    Sha256Assigner(const IPhotoInfo::Ptr& photoInfo): ITaskExecutor::ITask(), m_photoInfo(photoInfo)
    {
    }

    Sha256Assigner(const Sha256Assigner &) = delete;
    Sha256Assigner& operator=(const Sha256Assigner &) = delete;

    virtual std::string name() const override
    {
        return "Photo hash generation";
    }

    virtual void perform() override
    {
        const QByteArray data = PhotosManager::instance()->getPhoto(m_photoInfo);

        const unsigned char* udata = reinterpret_cast<const unsigned char *>(data.constData());
        const IPhotoInfo::Sha256sum hash = HashFunctions::sha256(udata, data.size());
        m_photoInfo->initSha256(hash);
    }

    IPhotoInfo::Ptr m_photoInfo;
};


struct TagsCollector: public ITaskExecutor::ITask
{
    TagsCollector(const IPhotoInfo::Ptr& photoInfo) : ITaskExecutor::ITask(), m_photoInfo(photoInfo), m_tagFeederFactory(nullptr)
    {
    }

    TagsCollector(const TagsCollector &) = delete;
    TagsCollector& operator=(const TagsCollector &) = delete;

    void set(ITagFeederFactory* tagFeederFactory)
    {
        m_tagFeederFactory = tagFeederFactory;
    }

    virtual std::string name() const override
    {
        return "Photo tags collection";
    }

    virtual void perform() override
    {
        const QString& path = m_photoInfo->getPath();
        std::shared_ptr<ITagFeeder> feeder = m_tagFeederFactory->get();
        Tag::TagsList p_tags = feeder->getTagsFor(path);

        m_photoInfo->setTags(p_tags);
        m_photoInfo->markFlag(IPhotoInfo::FlagsE::ExifLoaded, 1);
    }

    IPhotoInfo::Ptr m_photoInfo;
    ITagFeederFactory* m_tagFeederFactory;
};


PhotoInfoUpdater::PhotoInfoUpdater(): m_tagFeederFactory(), m_taskQueue(), m_configuration(nullptr)
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateSha256(const IPhotoInfo::Ptr& photoInfo)
{
    std::unique_ptr<Sha256Assigner> task(new Sha256Assigner(photoInfo));

    m_taskQueue->push(std::move(task));
}


void PhotoInfoUpdater::updateThumbnail(const IPhotoInfo::Ptr& photoInfo)
{
    QVariant widthEntry = m_configuration->getEntry(ConfigConsts::BasicKeys::thumbnailWidth);
    int width = 120;

    if (widthEntry.isValid())
        width = widthEntry.toInt();

    std::unique_ptr<ThumbnailGenerator> task(new ThumbnailGenerator(photoInfo, width));

    m_taskQueue->push(std::move(task));
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    std::unique_ptr<TagsCollector> task(new TagsCollector(photoInfo));
    task->set(&m_tagFeederFactory);

    m_taskQueue->push(std::move(task));
}


void PhotoInfoUpdater::set(ITaskExecutor* taskExecutor)
{
    m_taskQueue = taskExecutor->getCustomTaskQueue();
}


void PhotoInfoUpdater::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


int PhotoInfoUpdater::tasksInProgress()
{
    return m_taskQueue != nullptr? m_taskQueue->size() : 0;
}


void PhotoInfoUpdater::dropPendingTasks()
{
    if (m_taskQueue != nullptr)
        m_taskQueue->clear();
}
