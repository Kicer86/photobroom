
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
#include <core/ilogger_factory.hpp>
#include <core/ilogger.hpp>
#include <core/imedia_information.hpp>
#include <core/tag.hpp>
#include <core/task_executor.hpp>

#include "database/general_flags.hpp"
#include "photos_analyzer_constants.hpp"

// TODO: unit tests

using namespace PhotosAnalyzerConsts;

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

    FileInformation getFileInformation(const QString& path)
    {
        return m_updater->getFileInformation(path);
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
                         const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
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
            const FileInformation info = getFileInformation(path);

            auto photoDelta = m_photoInfo->lock();
            if (info.common.dimension.has_value())
            {
                photoDelta->geometry = info.common.dimension.value();
                photoDelta->flags[Photo::FlagsE::GeometryLoaded] = GeometryFlagVersion;
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
    };


    struct TagsCollector: UpdaterTask
    {
        TagsCollector(PhotoInfoUpdater* updater, const Photo::SharedData& photoInfo)
            : UpdaterTask(updater, photoInfo)
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
            // make sure we wan't to update
            QString path;
            {
                auto lockedPhoto = m_photoInfo->lock();
                const auto tags = lockedPhoto->tags;
                path = lockedPhoto->path;

                // If media already has date or time update, do not override it.
                // Just update ExifLoaded flag. It could be set to previous version, so bump it
                if (tags.contains(TagTypes::Date) || tags.contains(TagTypes::Time))
                {
                    lockedPhoto->flags[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
                    return;
                }
            }

            // collect data
            const FileInformation info = getFileInformation(path);

            // lock photo again (we do not need to keep resource unavailable during 'getFileInformation')
            auto photoDelta = m_photoInfo->lock();
            if (info.common.creationTime.has_value())
            {
                Tag::TagsList tags = photoDelta->tags;

                tags[TagTypes::Date] = info.common.creationTime->date();
                tags[TagTypes::Time] = info.common.creationTime->time();

                photoDelta->tags = tags;
            }

            photoDelta->flags[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
        }
    };

}


PhotoInfoUpdater::PhotoInfoUpdater(ITaskExecutor& executor, IMediaInformation& mediaInformation, ICoreFactoryAccessor* coreFactory, Database::IDatabase& db):
    m_logger(coreFactory->getLoggerFactory().get("PhotoInfoUpdater")),
    m_mediaInformation(mediaInformation),
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
    auto task = std::make_unique<GeometryAssigner>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateTags(const Photo::SharedData& photoInfo)
{
    auto task = std::make_unique<TagsCollector>(this, photoInfo);

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


FileInformation PhotoInfoUpdater::getFileInformation(const QString& path)
{
    std::lock_guard _(m_fileInfosMutex);

    FileInformation* info = m_fileInfos.object(path);
    if (info == nullptr)
    {
        auto fetchedInfo = std::make_unique<FileInformation>(m_mediaInformation.getInformation(path));
        info = fetchedInfo.get();
        m_fileInfos.insert(path, fetchedInfo.release());
    }

    return *info;
}
