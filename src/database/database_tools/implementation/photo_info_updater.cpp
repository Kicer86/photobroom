
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

        std::string name() const override
        {
            return "Photo hash generation";
        }

        void perform() override
        {
            QFile file(m_photoInfo->lock()->path);
            bool status = file.open(QFile::ReadOnly);
            assert(status);

            QCryptographicHash hasher(QCryptographicHash::Sha256);
            status = hasher.addData(&file);
            assert(status);

            const QByteArray rawHash = hasher.result();
            const QByteArray hexHash = rawHash.toHex();

            assert(hexHash.isEmpty() == false);

            auto photoDelta = m_photoInfo->lock();
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

        std::string name() const override
        {
            return "Photo geometry setter";
        }

        void perform() override
        {
            const QString path = m_photoInfo->lock()->path;
            const FileInformation info = m_mediaInformation->getInformation(path);

            auto photoDelta = m_photoInfo->lock();
            if (info.common.dimension.isValid())
            {
                photoDelta->geometry = info.common.dimension;
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
        TagsCollector(PhotoInfoUpdater* updater, IMediaInformation& mediaInformation, const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
            , m_mediaInformation(mediaInformation)
        {

        }

        TagsCollector(const TagsCollector &) = delete;
        TagsCollector& operator=(const TagsCollector &) = delete;

        std::string name() const override
        {
            return "Photo tags collection";
        }

        void perform() override
        {
            // collect data
            const QString path = m_photoInfo->lock()->path;
            const FileInformation info = m_mediaInformation.getInformation(path);

            auto photoDelta = m_photoInfo->lock();

            if (info.common.creationTime.isValid())
            {
                Tag::TagsList tags = photoDelta->tags;

                tags[TagTypes::Date] = info.common.creationTime.date();
                tags[TagTypes::Time] = info.common.creationTime.time();

                photoDelta->tags = tags;
            }

            photoDelta->flags[Photo::FlagsE::ExifLoaded] = 1;
        }

        IMediaInformation& m_mediaInformation;
    };

}


PhotoInfoUpdater::PhotoInfoUpdater(ITaskExecutor& executor, ICoreFactoryAccessor* coreFactory, Database::IDatabase& db):
    m_mediaInformation(coreFactory),
    m_logger(coreFactory->getLoggerFactory().get("PhotoInfoUpdater")),
    m_coreFactory(coreFactory),
    m_db(db),
    m_tasksExecutor(executor)
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

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
    auto task = std::make_unique<TagsCollector>(this, m_mediaInformation, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::addTask(std::unique_ptr<UpdaterTask> task)
{
    m_tasksExecutor.add(std::move(task));
}


void PhotoInfoUpdater::applyFlags(const Photo::Id& id, const std::pair<QString, int>& generic_flag)
{
    m_db.exec([id, generic_flag](Database::IBackend& db)
    {
        db.set(id, generic_flag.first, generic_flag.second);
    });
}
