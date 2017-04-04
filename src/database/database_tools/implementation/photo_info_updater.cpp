
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QPixmap>
#include <QImage>
#include <QCryptographicHash>

#include <core/task_executor.hpp>
#include <core/photos_manager.hpp>
#include <core/iexif_reader.hpp>
#include <core/iphoto_information.hpp>
#include <core/tag.hpp>


struct UpdaterTask: ITaskExecutor::ITask
{
    UpdaterTask(PhotoInfoUpdater* updater): m_updater(updater)
    {
        m_updater->taskAdded(this);
    }

    virtual ~UpdaterTask()
    {
        m_updater->taskFinished(this);
    }

    UpdaterTask(const UpdaterTask &) = delete;
    UpdaterTask& operator=(const UpdaterTask &) = delete;

    PhotoInfoUpdater* m_updater;
};


namespace
{

    struct Sha256Assigner: UpdaterTask
    {
        Sha256Assigner(PhotoInfoUpdater* updater,
                       IPhotosManager* photosManager,
                       const IPhotoInfo::Ptr& photoInfo):
            UpdaterTask(updater),
            m_photoInfo(photoInfo),
            m_photosManager(photosManager)
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
            const QString path = m_photoInfo->getPath();
            const QByteArray data = m_photosManager->getPhoto(path);
            const QByteArray rawHash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
            const QByteArray hexHash = rawHash.toHex();

            m_photoInfo->setSha256(hexHash);
        }

        IPhotoInfo::Ptr m_photoInfo;
        IPhotosManager* m_photosManager;
    };


    struct GeometryAssigner: UpdaterTask
    {
        GeometryAssigner(PhotoInfoUpdater* updater,
                         IPhotoInformation* photoInformation,
                         const IPhotoInfo::Ptr& photoInfo):
            UpdaterTask(updater),
            m_photoInfo(photoInfo),
            m_photoInformation(photoInformation)
        {
        }

        GeometryAssigner(const GeometryAssigner &) = delete;
        GeometryAssigner& operator=(const GeometryAssigner &) = delete;

        virtual std::string name() const override
        {
            return "Photo geometry setter";
        }

        virtual void perform() override
        {
            const QString path = m_photoInfo->getPath();
            const QSize size = m_photoInformation->size(path);

            m_photoInfo->setGeometry(size);
        }

        IPhotoInfo::Ptr m_photoInfo;
        IPhotoInformation* m_photoInformation;
    };

}

struct TagsCollector: UpdaterTask
{
    TagsCollector(PhotoInfoUpdater* updater, const IPhotoInfo::Ptr& photoInfo): UpdaterTask(updater), m_photoInfo(photoInfo), m_exifReaderFactory (nullptr)
    {
    }

    TagsCollector(const TagsCollector &) = delete;
    TagsCollector& operator=(const TagsCollector &) = delete;

    void set(IExifReaderFactory* exifReaderFactory)
    {
        m_exifReaderFactory = exifReaderFactory;
    }

    virtual std::string name() const override
    {
        return "Photo tags collection";
    }

    virtual void perform() override
    {
        const QString& path = m_photoInfo->getPath();
        IExifReader* feeder = m_exifReaderFactory->get();
        Tag::TagsList p_tags = feeder->getTagsFor(path);

        m_photoInfo->setTags(p_tags);
        m_photoInfo->markFlag(Photo::FlagsE::ExifLoaded, 1);
    }

    IPhotoInfo::Ptr m_photoInfo;
    IExifReaderFactory* m_exifReaderFactory;
};


PhotoInfoUpdater::PhotoInfoUpdater():
    m_photoInformation(),
    m_exifReaderFactory(),
    m_taskQueue(),
    m_tasks(),
    m_tasksMutex(),
    m_finishedTask(),
    m_configuration(nullptr),
    m_photosManager(nullptr)
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateSha256(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<Sha256Assigner>(this, m_photosManager, photoInfo);

    m_taskQueue->push(std::move(task));
}


void PhotoInfoUpdater::updateGeometry(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<GeometryAssigner>(this, &m_photoInformation, photoInfo);

    m_taskQueue->push(std::move(task));
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<TagsCollector>(this, photoInfo);
    task->set(&m_exifReaderFactory);

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


void PhotoInfoUpdater::set(IPhotosManager* photosManager)
{
    m_photosManager = photosManager;
    m_exifReaderFactory.set(photosManager);

    m_photoInformation.set(m_exifReaderFactory.get());
}


int PhotoInfoUpdater::tasksInProgress()
{
    return m_tasks.size();
}


void PhotoInfoUpdater::dropPendingTasks()
{
    if (m_taskQueue != nullptr)
        m_taskQueue->clear();
}


void PhotoInfoUpdater::waitForActiveTasks()
{
    std::unique_lock<std::mutex> lock(m_tasksMutex);
    m_finishedTask.wait(lock, [&]
    {
        return m_tasks.empty();
    });
}


void PhotoInfoUpdater::taskAdded(UpdaterTask* task)
{
    std::lock_guard<std::mutex> lock(m_tasksMutex);
    m_tasks.insert(task);
}


void PhotoInfoUpdater::taskFinished(UpdaterTask* task)
{
    {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        m_tasks.erase(task);
    }

    m_finishedTask.notify_one();
}
