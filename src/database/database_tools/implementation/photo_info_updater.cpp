
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>
#include <ranges>
#include <opencv2/img_hash.hpp>
#include <QImage>
#include <QFile>
#include <QPixmap>

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
    UpdaterTask(PhotoInfoUpdater* updater, const Photo::SharedDataDelta& delta)
        : m_updater(updater)
        , m_photoInfo(delta)
    {

    }

    virtual ~UpdaterTask()
    {

    }

    void apply(std::function<void(Database::IBackend &)> action)
    {
        invokeMethod(m_updater, &PhotoInfoUpdater::apply, action);
    }

    FileInformation getFileInformation(const QString& path)
    {
        return m_updater->getFileInformation(path);
    }

    UpdaterTask(const UpdaterTask &) = delete;
    UpdaterTask& operator=(const UpdaterTask &) = delete;

    PhotoInfoUpdater* m_updater;
    Photo::SharedDataDelta m_photoInfo;
};


namespace
{

    struct GeometryAssigner: UpdaterTask
    {
        GeometryAssigner(PhotoInfoUpdater* updater,
                         const Photo::SharedDataDelta& photoInfo)
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
            const QString path = m_photoInfo->lock()->get<Photo::Field::Path>();
            const FileInformation info = getFileInformation(path);

            auto photoDelta = m_photoInfo->lock();
            if (info.common.dimension.has_value())
            {
                photoDelta->insert<Photo::Field::Geometry>(info.common.dimension.value());
                photoDelta->get<Photo::Field::Flags>()[Photo::FlagsE::GeometryLoaded] = GeometryFlagVersion;
            }
            else
                apply([id = photoDelta->getId()](Database::IBackend& backend)
                {
                    backend.setBits(id, Database::CommonGeneralFlags::State, static_cast<int>(Database::CommonGeneralFlags::StateType::Broken));
                });
        }
    };


    struct TagsCollector: UpdaterTask
    {
        TagsCollector(PhotoInfoUpdater* updater, const Photo::SharedDataDelta& photoInfo)
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
            // make sure we want to update
            QString path;
            {
                auto lockedPhoto = m_photoInfo->lock();
                const auto tags = lockedPhoto->get<Photo::Field::Tags>();
                path = lockedPhoto->get<Photo::Field::Path>();

                // If media already has date or time update, do not override it.
                // Just update ExifLoaded flag. It could be set to previous version, so bump it
                if (tags.contains(Tag::Types::Date) || tags.contains(Tag::Types::Time))
                {
                    lockedPhoto->get<Photo::Field::Flags>()[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
                    return;
                }
            }

            // collect data
            const FileInformation info = getFileInformation(path);

            // lock photo again (we do not need to keep resource unavailable during 'getFileInformation')
            auto photoDelta = m_photoInfo->lock();
            if (info.common.creationTime.has_value())
            {
                Tag::TagsList& tags = photoDelta->get<Photo::Field::Tags>();

                tags[Tag::Types::Date] = info.common.creationTime->date();
                tags[Tag::Types::Time] = info.common.creationTime->time();
            }

            photoDelta->get<Photo::Field::Flags>()[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
        }
    };


    struct PHashCalculator: UpdaterTask
    {
        PHashCalculator(PhotoInfoUpdater* updater, const Photo::SharedDataDelta& photoInfo)
            : UpdaterTask(updater, photoInfo)
        {

        }

        std::string name() const override
        {
            return "PHash calculator";
        }

        void perform() override
        {
            // based on:
            // https://docs.opencv.org/3.4/d4/d93/group__img__hash.html

            const QString path = m_photoInfo->lock()->get<Photo::Field::Path>();

            // NOTE: cv::imread could be used here, however it would be better to have a unique mechanism
            // of reading images, so if an image can be displayed in gui, then we also know how to
            // read and phash it here.
            QImage image(path);

            if (image.isNull())
                apply([id = m_photoInfo->lock()->getId()](Database::IBackend& backend)
                {
                    backend.setBits(id, Database::CommonGeneralFlags::PHashState, static_cast<int>(Database::CommonGeneralFlags::PHashStateType::Incomaptible));
                });
            else
            {
                if (image.format() != QImage::Format_ARGB32)
                    image = image.convertToFormat(QImage::Format_ARGB32);

                const cv::Mat cvImage(
                    image.height(),
                    image.width(),
                    CV_8UC4,
                    image.bits(),
                    static_cast<std::size_t>(image.bytesPerLine())
                );

                cv::Mat phashMat;
                cv::img_hash::pHash(cvImage, phashMat);

                constexpr int DataSize = 8;
                assert( phashMat.rows == 1);
                assert( phashMat.cols == DataSize);

                const auto count = phashMat.dataend - phashMat.datastart;

                if (count == DataSize)
                {
                    std::array<std::byte, DataSize> rawPHash;
                    std::memcpy(rawPHash.data(), phashMat.datastart, DataSize);

                    Photo::PHash phash(rawPHash);
                    m_photoInfo->lock()->insert<Photo::Field::PHash>(phash);
                }
            }
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


void PhotoInfoUpdater::updateGeometry(const Photo::SharedDataDelta& photoInfo)
{
    auto task = std::make_unique<GeometryAssigner>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updateTags(const Photo::SharedDataDelta& photoInfo)
{
    auto task = std::make_unique<TagsCollector>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::updatePHash(const Photo::SharedDataDelta& photoInfo)
{
    auto task = std::make_unique<PHashCalculator>(this, photoInfo);

    addTask(std::move(task));
}


void PhotoInfoUpdater::addTask(std::unique_ptr<UpdaterTask> task)
{
    m_tasksExecutor.add(std::move(task));
}


void PhotoInfoUpdater::apply(std::function<void(Database::IBackend &)> action)
{
    m_db.exec(action);
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
