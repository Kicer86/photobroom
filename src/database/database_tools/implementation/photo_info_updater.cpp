
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QImage>
#include <QFile>
#include <QPixmap>
#include <QCryptographicHash>

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/iconfiguration.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/imedia_information.hpp>
#include <core/map_iterator.hpp>
#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "database/general_flags.hpp"

// TODO: unit tests

struct UpdaterTask: ITaskExecutor::ITask
{
    UpdaterTask(PhotoInfoUpdater* updater): m_updater(updater)
    {

    }

    virtual ~UpdaterTask()
    {
        m_updater->taskFinished(this);
    }

    void apply(const Photo::DataDelta& delta)
    {
        invokeMethod(m_updater, &PhotoInfoUpdater::apply, delta);
    }

    void apply(const Photo::Id& id, const std::pair<QString, int>& generic_flag)
    {
        invokeMethod(m_updater, &PhotoInfoUpdater::applyFlags, id, generic_flag);
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
                       const Photo::Data& photoInfo):
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
            QFile file(m_photoInfo.path);
            bool status = file.open(QFile::ReadOnly);
            assert(status);

            QCryptographicHash hasher(QCryptographicHash::Sha256);
            status = hasher.addData(&file);
            assert(status);

            const QByteArray rawHash = hasher.result();
            const QByteArray hexHash = rawHash.toHex();

            assert(hexHash.isEmpty() == false);

            Photo::DataDelta delta(m_photoInfo.id);
            delta.insert<Photo::Field::Checksum>(hexHash);
            delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::Sha256Loaded, 1}} );

            apply(delta);
        }

        Photo::Data m_photoInfo;
    };


    struct GeometryAssigner: UpdaterTask
    {
        GeometryAssigner(PhotoInfoUpdater* updater,
                         IMediaInformation* photoInformation,
                         const Photo::Data& photoInfo):
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
            const std::optional<QSize> size = m_mediaInformation->size(m_photoInfo.path);

            if (size.has_value())
            {
                Photo::DataDelta delta(m_photoInfo.id);
                delta.insert<Photo::Field::Geometry>(*size);
                delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::GeometryLoaded, 1}} );

                apply(delta);
            }
            else
            {
                Photo::DataDelta delta(m_photoInfo.id);

                apply(m_photoInfo.id, {
                    Database::CommonGeneralFlags::State,
                    static_cast<int>(Database::CommonGeneralFlags::StateType::Broken)
                });
            }
        }

        Photo::Data m_photoInfo;
        IMediaInformation* m_mediaInformation;
    };


    struct TagsCollector: UpdaterTask
    {
        TagsCollector(PhotoInfoUpdater* updater, IExifReaderFactory& exifReaderFactory, const Photo::Data& photoInfo):
            UpdaterTask(updater),
            m_photoInfo(photoInfo),
            m_exifReaderFactory(exifReaderFactory)
        {
        }

        TagsCollector(const TagsCollector &) = delete;
        TagsCollector& operator=(const TagsCollector &) = delete;

        virtual std::string name() const override
        {
            return "Photo tags collection";
        }

        virtual void perform() override
        {
            IExifReader& feeder = m_exifReaderFactory.get();

            // merge found tags with current tags.
            const Tag::TagsList new_tags = feeder.getTagsFor(m_photoInfo.path);
            const Tag::TagsList cur_tags = m_photoInfo.tags;

            Tag::TagsList tags = cur_tags;

            for (const auto& entry: new_tags)
            {
                auto it = tags.find(entry.first);

                if (it == tags.end())   // no such tag yet?
                    tags.insert(entry);
            }

            Photo::DataDelta delta(m_photoInfo.id);
            delta.insert<Photo::Field::Tags>(tags);
            delta.insert<Photo::Field::Flags>( {{Photo::FlagsE::ExifLoaded, 1}} );

            apply(delta);
        }

        Photo::Data m_photoInfo;
        IExifReaderFactory& m_exifReaderFactory;
    };

}


PhotoInfoUpdater::PhotoInfoUpdater(ICoreFactoryAccessor* coreFactory, Database::IDatabase& db):
    m_mediaInformation(coreFactory),
    m_tasks(),
    m_tasksMutex(),
    m_finishedTask(),
    m_threadId(std::this_thread::get_id()),
    m_logger(coreFactory->getLoggerFactory().get("PhotoInfoUpdater")),
    m_coreFactory(coreFactory),
    m_db(db),
    m_tasksExecutor(coreFactory->getTaskExecutor())
{
    m_cacheFlushTimer.setSingleShot(true);

    connect(&m_cacheFlushTimer, &QTimer::timeout, this, &PhotoInfoUpdater::flushCache);
}


PhotoInfoUpdater::~PhotoInfoUpdater()
{
    waitForActiveTasks();
}


void PhotoInfoUpdater::updateSha256(const Photo::Data& photoInfo)
{
    auto task = std::make_unique<Sha256Assigner>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateGeometry(const Photo::Data& photoInfo)
{
    auto task = std::make_unique<GeometryAssigner>(this, &m_mediaInformation, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateTags(const Photo::Data& photoInfo)
{
    auto task = std::make_unique<TagsCollector>(this, m_coreFactory->getExifReaderFactory(), photoInfo);

    addTask(std::move(task));
}


int PhotoInfoUpdater::tasksInProgress()
{
    return static_cast<int>(m_tasks.size());
}



void PhotoInfoUpdater::waitForActiveTasks()
{
    std::unique_lock<std::mutex> lock(m_tasksMutex);
    m_finishedTask.wait(lock, [&]
    {
        return m_tasks.empty();
    });
}


void PhotoInfoUpdater::addTask(std::unique_ptr<UpdaterTask> task)
{
    std::lock_guard<std::mutex> lock(m_tasksMutex);
    m_tasks.insert(task.get());
    m_tasksExecutor.add(std::move(task));
}


void PhotoInfoUpdater::taskFinished(UpdaterTask* task)
{
    {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        m_tasks.erase(task);
    }

    m_finishedTask.notify_one();

    emit photoProcessed();
}


void PhotoInfoUpdater::apply(const Photo::DataDelta& delta)
{
    assert(m_threadId == std::this_thread::get_id());

    m_touchedPhotos[delta.getId()] |= delta;

    // when cache has too many changes, let timer fire to flush cache
    const bool runs = m_cacheFlushTimer.isActive();
    const bool canKick = m_cacheFlushTimer.interval() - m_cacheFlushTimer.remainingTime() > 1000; // time since last kick > 1s?
    const std::size_t count = m_touchedPhotos.size();

    if (count < 500 && (runs == false || canKick))
    {
        resetFlushTimer();
        m_logger->debug(QString("Restarting flush timer. %1 photos waiting for flush").arg(count));
    }
}


void PhotoInfoUpdater::applyFlags(const Photo::Id& id, const std::pair<QString, int>& generic_flag)
{
    m_db.exec([id, generic_flag](Database::IBackend& db)
    {
        db.set(id, generic_flag.first, generic_flag.second);
    });
}


void PhotoInfoUpdater::flushCache()
{
    if (m_touchedPhotos.empty() == false)
    {
        m_logger->debug(QString("Sending %1 photos to update").arg(m_touchedPhotos.size()));

        m_db.exec([delta = std::move(m_touchedPhotos)](Database::IBackend& db)
        {
            const std::vector<Photo::DataDelta> vectorOfDeltas(value_map_iterator<TouchedPhotos>(delta.cbegin()),
                                                               value_map_iterator<TouchedPhotos>(delta.cend()));

            db.update(vectorOfDeltas);
        });
    }
}


void PhotoInfoUpdater::resetFlushTimer()
{
    m_cacheFlushTimer.start(5000);
}
