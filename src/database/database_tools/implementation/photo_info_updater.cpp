
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>
#include <ranges>

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
#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "database/general_flags.hpp"

// TODO: unit tests

struct UpdaterTask: ITaskExecutor::ITask
{
    UpdaterTask(PhotoInfoUpdater* updater, const Photo::SharedData& delta)
        : m_updater(updater)
        , m_photoInfo(delta)
    {

    }

    virtual ~UpdaterTask()
    {
        m_updater->taskFinished(this);
    }

    void apply(const Photo::Id& id, const std::pair<QString, int>& generic_flag)
    {
        invokeMethod(m_updater, &PhotoInfoUpdater::applyFlags, id, generic_flag);
    }

    UpdaterTask(const UpdaterTask &) = delete;
    UpdaterTask& operator=(const UpdaterTask &) = delete;

    PhotoInfoUpdater* m_updater;
    Photo::SharedData m_photoInfo;
};


namespace
{

    struct Sha256Assigner: UpdaterTask
    {
        Sha256Assigner(PhotoInfoUpdater* updater,
                       const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
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
            auto photoDelta = m_photoInfo->lock();

            QFile file(photoDelta->path);
            bool status = file.open(QFile::ReadOnly);
            assert(status);

            QCryptographicHash hasher(QCryptographicHash::Sha256);
            status = hasher.addData(&file);
            assert(status);

            const QByteArray rawHash = hasher.result();
            const QByteArray hexHash = rawHash.toHex();

            assert(hexHash.isEmpty() == false);

            photoDelta->sha256Sum = hexHash;
            photoDelta->flags[Photo::FlagsE::Sha256Loaded] = 1;
        }
    };


    struct GeometryAssigner: UpdaterTask
    {
        GeometryAssigner(PhotoInfoUpdater* updater,
                         IMediaInformation* photoInformation,
                         const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
            , m_mediaInformation(photoInformation)
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
            auto photoDelta = m_photoInfo->lock();

            const std::optional<QSize> size = m_mediaInformation->size(photoDelta->path);

            if (size.has_value())
            {
                photoDelta->geometry = *size;
                photoDelta->flags[Photo::FlagsE::GeometryLoaded] = 1;

            }
            else
            {
                apply(photoDelta->id,
                {
                    Database::CommonGeneralFlags::State,
                    static_cast<int>(Database::CommonGeneralFlags::StateType::Broken)
                });
            }
        }

        IMediaInformation* m_mediaInformation;
    };


    struct TagsCollector: UpdaterTask
    {
        TagsCollector(PhotoInfoUpdater* updater, IExifReaderFactory& exifReaderFactory, const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
            , m_exifReaderFactory(exifReaderFactory)
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
            auto photoDelta = m_photoInfo->lock();

            // collect data
            const Tag::TagsList new_tags = feeder.getTagsFor(photoDelta->path);
            const Tag::TagsList& cur_tags = photoDelta->tags;

            // merge new_tags with cur_tags
            Tag::TagsList tags = cur_tags;

            for (const auto& entry: new_tags)
            {
                auto it = tags.find(entry.first);

                if (it == tags.end())   // no such tag yet?
                    tags.insert(entry);
            }

            photoDelta->flags[Photo::FlagsE::ExifLoaded] = 1;
            photoDelta->tags = tags;
        }

        IExifReaderFactory& m_exifReaderFactory;
    };

}


PhotoInfoUpdater::PhotoInfoUpdater(ICoreFactoryAccessor* coreFactory, Database::IDatabase& db):
    m_mediaInformation(coreFactory),
    m_tasks(),
    m_tasksMutex(),
    m_finishedTask(),
    m_logger(coreFactory->getLoggerFactory().get("PhotoInfoUpdater")),
    m_coreFactory(coreFactory),
    m_db(db),
    m_tasksExecutor(coreFactory->getTaskExecutor())
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{
    waitForActiveTasks();
}


void PhotoInfoUpdater::updateSha256(const Photo::SharedData& photoInfo)
{
    auto task = std::make_unique<Sha256Assigner>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateGeometry(const Photo::SharedData& photoInfo)
{
    auto task = std::make_unique<GeometryAssigner>(this, &m_mediaInformation, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateTags(const Photo::SharedData& photoInfo)
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
    {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        m_tasks.insert(task.get());
    }

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


void PhotoInfoUpdater::applyFlags(const Photo::Id& id, const std::pair<QString, int>& generic_flag)
{
    m_db.exec([id, generic_flag](Database::IBackend& db)
    {
        db.set(id, generic_flag.first, generic_flag.second);
    });
}
