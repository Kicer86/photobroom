
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QImage>
#include <QFile>
#include <QPixmap>
#include <QCryptographicHash>

#include <core/icore_factory_accessor.hpp>
#include <core/iconfiguration.hpp>
#include <core/iexif_reader.hpp>
#include <core/imedia_information.hpp>
#include <core/tag.hpp>
#include <core/task_executor.hpp>

// TODO: unit tests

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
                       const IPhotoInfo::Ptr& photoInfo):
            UpdaterTask(updater),
            m_photoInfo(photoInfo)
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

            QFile file(path);
            bool status = file.open(QFile::ReadOnly);
            assert(status);

            QCryptographicHash hasher(QCryptographicHash::Sha256);
            status = hasher.addData(&file);
            assert(status);

            const QByteArray rawHash = hasher.result();
            const QByteArray hexHash = rawHash.toHex();

            assert(hexHash.isEmpty() == false);
            m_photoInfo->setSha256(hexHash);

            Photo::DataDelta delta(m_photoInfo->getID());
            delta.insert<Photo::Field::Checksum>(hexHash);
            delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::Sha256Loaded, 1}} );

            m_updater->apply(delta);
        }

        IPhotoInfo::Ptr m_photoInfo;
    };


    struct GeometryAssigner: UpdaterTask
    {
        GeometryAssigner(PhotoInfoUpdater* updater,
                         IMediaInformation* photoInformation,
                         const IPhotoInfo::Ptr& photoInfo):
            UpdaterTask(updater),
            m_photoInfo(photoInfo),
            m_mediaInformation(photoInformation)
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
            const std::optional<QSize> size = m_mediaInformation->size(path);

            if (size.has_value())
            {
                m_photoInfo->setGeometry(*size);

                Photo::DataDelta delta(m_photoInfo->getID());
                delta.insert<Photo::Field::Geometry>(*size);
                delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::GeometryLoaded, 1}} );

                m_updater->apply(delta);
            }
        }

        IPhotoInfo::Ptr m_photoInfo;
        IMediaInformation* m_mediaInformation;
    };


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

            // merge found tags with current tags.
            const Tag::TagsList new_tags = feeder->getTagsFor(path);
            const Tag::TagsList cur_tags = m_photoInfo->getTags();

            Tag::TagsList tags = cur_tags;

            for (const auto& entry: new_tags)
            {
                auto it = tags.find(entry.first);

                if (it == tags.end())   // no such tag yet?
                    tags.insert(entry);
            }

            m_photoInfo->setTags(tags);
            m_photoInfo->markFlag(Photo::FlagsE::ExifLoaded, 1);

            Photo::DataDelta delta(m_photoInfo->getID());
            delta.insert<Photo::Field::Tags>(tags);
            delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::ExifLoaded, 1}} );

            m_updater->apply(delta);
        }

        IPhotoInfo::Ptr m_photoInfo;
        IExifReaderFactory* m_exifReaderFactory;
    };

}


PhotoInfoUpdater::PhotoInfoUpdater( ICoreFactoryAccessor* coreFactory, Database::IDatabase* db):
    m_mediaInformation(),
    m_taskQueue(coreFactory->getTaskExecutor()),
    m_tasks(),
    m_tasksMutex(),
    m_finishedTask(),
    m_coreFactory(coreFactory),
    m_db(db)
{
    m_mediaInformation.set(coreFactory);
    m_cacheFlushTimer.setSingleShot(true);

    connect(&m_cacheFlushTimer, &QTimer::timeout, this, &PhotoInfoUpdater::flushCache);
}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateSha256(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<Sha256Assigner>(this, photoInfo);

    m_taskQueue.push(std::move(task));
}


void PhotoInfoUpdater::updateGeometry(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<GeometryAssigner>(this, &m_mediaInformation, photoInfo);

    m_taskQueue.push(std::move(task));
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_unique<TagsCollector>(this, photoInfo);
    task->set(m_coreFactory->getExifReaderFactory());

    m_taskQueue.push(std::move(task));
}


int PhotoInfoUpdater::tasksInProgress()
{
    return static_cast<int>(m_tasks.size());
}


void PhotoInfoUpdater::dropPendingTasks()
{
    m_taskQueue.clear();
}


void PhotoInfoUpdater::waitForActiveTasks()
{
    std::unique_lock<std::mutex> lock(m_tasksMutex);
    m_finishedTask.wait(lock, [&]
    {
        return m_tasks.empty();
    });
}


void PhotoInfoUpdater::apply(const Photo::DataDelta& delta)
{
    std::lock_guard lock(m_touchedPhotosMutex);

    m_touchedPhotos[delta.getId()] |= delta;

    // when cache has too many changes, let timer fire to flush cache
    if (m_touchedPhotos.size() < 500)
        resetFlushTimer();
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


void PhotoInfoUpdater::flushCache()
{
    std::lock_guard lock(m_touchedPhotosMutex);

    if (m_touchedPhotos.empty() == false)
    {
        m_db->exec([delta = std::move(m_touchedPhotos)](Database::IBackend& db)
        {
            (void) db;
            (void) delta;
        });
    }
}


void PhotoInfoUpdater::resetFlushTimer()
{
    m_cacheFlushTimer.start(5000);
}
