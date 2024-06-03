/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <opencv2/img_hash.hpp>

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/itask_executor.hpp>
#include <database/iphoto_operator.hpp>
#include <database/general_flags.hpp>
#include <database/photo_flags_values.hpp>

#include "photos_analyzer_p.hpp"
#include "../photos_analyzer.hpp"


using namespace std::chrono_literals;
using namespace std::placeholders;
using namespace PhotosAnalyzerConsts;


namespace
{
    // TODO: put some nice concept here which will detect if T is ExplicitDelta and has required fields
    void assignGeometry(IMediaInformation& mediaInfo, auto& data)
    {
        const QString path = data.template get<Photo::Field::Path>();
        const auto info = mediaInfo.getInformation(path);

        if (info.common.dimension.has_value())
        {
            data.template insert<Photo::Field::Geometry>(info.common.dimension.value());
            data.template get<Photo::Field::Flags>()[Photo::FlagsE::GeometryLoaded] = GeometryFlagVersion;
        }
        else
            throw std::make_pair(Database::CommonGeneralFlags::State, static_cast<int>(Database::CommonGeneralFlags::StateType::Broken));
    }

    // TODO: put some nice concept here which will detect if T is ExplicitDelta and has required fields
    void assignTags(IMediaInformation& mediaInfo, auto& data)
    {
        auto& tags = data.template get<Photo::Field::Tags>();
        const auto path = data.template get<Photo::Field::Path>();

        // If media already has date or time update, do not override it.
        // Just update ExifLoaded flag. It could be set to previous version, so bump it
        if (tags.contains(Tag::Types::Date) || tags.contains(Tag::Types::Time))
        {
            data.template get<Photo::Field::Flags>()[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
            return;
        }

        // collect data
        const FileInformation info = mediaInfo.getInformation(path);

        if (info.common.creationTime.has_value())
        {
            tags[Tag::Types::Date] = info.common.creationTime->date();
            tags[Tag::Types::Time] = info.common.creationTime->time();
        }

        data.template get<Photo::Field::Flags>()[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;
    }

    // TODO: put some nice concept here which will detect if T is ExplicitDelta and has required fields
    void assignPHash(auto& data)
    {
        // based on:
        // https://docs.opencv.org/3.4/d4/d93/group__img__hash.html

        const QString path = data.template get<Photo::Field::Path>();

        // NOTE: cv::imread could be used here, however it would be better to have a unique mechanism
        // of reading images, so if an image can be displayed in gui, then we also know how to
        // read and phash it here.
        QImage image(path);

        if (image.isNull())
            throw std::make_pair(Database::CommonGeneralFlags::PHashState, static_cast<int>(Database::CommonGeneralFlags::PHashStateType::Incompatible));
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

                Photo::PHashT phash(rawPHash);
                data.template insert<Photo::Field::PHash>(phash);
            }
        }
    }

    struct UpdatePhoto: ITaskExecutor::ITask
    {
        UpdatePhoto(const std::shared_ptr<Database::DatabaseQueue>& storage, const Photo::Id& id, IMediaInformation& mediaInfo)
            : m_storage(storage)
            , m_id(id)
            , m_mediaInfo(mediaInfo)
        {

        }

        void perform() override
        {
            auto data = evaluate(*m_storage, [this](Database::IBackend& backend)
            {
                using namespace Photo;
                return backend.getPhotoDelta<Field::Flags, Field::PHash, Field::Path, Field::Geometry, Field::Tags>(m_id);
            });

            std::vector<std::tuple<Photo::Id, QString, int>> bitsToSet;

            if (data.get<Photo::Field::Flags>().at(Photo::FlagsE::GeometryLoaded) < GeometryFlagVersion)
                try
                {
                    assignGeometry(m_mediaInfo, data);
                }
                catch(const std::pair<QString, int>& bits)
                {
                    bitsToSet.push_back(std::make_tuple(m_id, bits.first, bits.second));
                }

            if (data.get<Photo::Field::Flags>().at(Photo::FlagsE::ExifLoaded) < ExifFlagVersion)
                assignTags(m_mediaInfo, data);

            if (data.get<Photo::Field::PHash>().valid() == false)
                try
                {
                    assignPHash(data);
                }
                catch(const std::pair<QString, int>& bits)
                {
                    bitsToSet.push_back(std::make_tuple(m_id, bits.first, bits.second));
                }

            data.get<Photo::Field::Flags>()[Photo::FlagsE::StagingArea] = 0;

            m_storage->pushPackibleTask([data, bitsToSet](Database::IBackend& backend)
            {
                backend.update({data});

                if (bitsToSet.empty() == false)
                    for(const auto& bits: bitsToSet)
                        backend.setBits(std::get<0>(bits), std::get<1>(bits), std::get<2>(bits));
            });
        }

        std::string name() const override
        {
            return "Update new photo data";
        }

        const std::shared_ptr<Database::DatabaseQueue> m_storage;
        const Photo::Id m_id;
        IMediaInformation& m_mediaInfo;
    };
}


PhotosAnalyzerImpl::PhotosAnalyzerImpl(ICoreFactoryAccessor* coreFactory, Database::IDatabase& database):
    m_taskQueue(coreFactory->getTaskExecutor()),
    m_mediaInformation(coreFactory),
    m_database(database),
    m_tasksView(nullptr),
    m_viewTask(nullptr)
{
    //check for not fully initialized photos in database
    //TODO: use independent updaters here (issue #102)

    // GeometryLoaded < 1
    Database::FilterPhotosWithFlags geometryFilter;
    geometryFilter.comparison[Photo::FlagsE::GeometryLoaded] = Database::ComparisonOp::Less;
    geometryFilter.flags[Photo::FlagsE::GeometryLoaded] = GeometryFlagVersion;

    // ExifLoaded < 2
    Database::FilterPhotosWithFlags exifFilter;
    exifFilter.comparison[Photo::FlagsE::ExifLoaded] = Database::ComparisonOp::Less;
    exifFilter.flags[Photo::FlagsE::ExifLoaded] = ExifFlagVersion;

    // Staging Area == 1
    Database::FilterPhotosWithFlags stagingAreaFilter;
    stagingAreaFilter.comparison[Photo::FlagsE::StagingArea] = Database::ComparisonOp::Equal;
    stagingAreaFilter.flags[Photo::FlagsE::StagingArea] = StagingAreaSet;

    // group flag filters
    Database::GroupFilter flagsFilter = {geometryFilter, exifFilter, stagingAreaFilter};
    flagsFilter.mode = Database::LogicalOp::Or;

    // only normal photos
    const auto generalFlagsFilter = Database::getValidPhotosFilter();

    const Database::GroupFilter noExifOrGeometryFilter = {flagsFilter, generalFlagsFilter};

    // photos with no phash
    Database::FilterPhotosWithPHash phashFilter;
    Database::FilterNotMatchingFilter noPhashFilter(phashFilter);

    // photos with phash_state == 0 (Normal) flag
    Database::FilterPhotosWithGeneralFlag phashGeneralFlagFilter(
        Database::CommonGeneralFlags::PHashState,
        static_cast<int>(Database::CommonGeneralFlags::PHashStateType::Normal)
    );

    // group phash filters
    const Database::GroupFilter noPhashFilterGroup = { noPhashFilter, phashGeneralFlagFilter };

    // bind all filters together
    Database::GroupFilter filters = {noExifOrGeometryFilter, noPhashFilterGroup};
    filters.mode = Database::LogicalOp::Or;

    m_database.exec([this, filters](Database::IBackend& backend)
    {
        auto photos = backend.photoOperator().getPhotos(filters);

        if (photos.empty() == false)
            invokeMethod(this, &PhotosAnalyzerImpl::addPhotos, photos);

        // as all uninitialized photos were found.
        // start watching for any new photos added later.
        m_backendConnection = connect(&backend, &Database::IBackend::photosAdded,
                                      this, &PhotosAnalyzerImpl::addPhotos);
    },
    "PhotosAnalyzerImpl: fetching nonanalyzed photos"
    );

    // update progress bar
    connect(&m_taskQueue, &ObservableExecutor::awaitingTasksChanged, this, [this](int awaiting)
    {
        if (m_viewTask)
            m_viewTask->getProgressBar()->setValue(m_maxPhotos - awaiting);
    });
}


PhotosAnalyzerImpl::~PhotosAnalyzerImpl()
{
    finishProgressBar();
    stop();
}


void PhotosAnalyzerImpl::set(ITasksView* tasksView)
{
    m_tasksView = tasksView;
}


void PhotosAnalyzerImpl::addPhotos(const std::vector<Photo::Id>& ids)
{
    std::shared_ptr<Database::DatabaseQueue> storage = m_storageQueue.lock();

    if (storage == nullptr)
    {
        m_maxPhotos = static_cast<int>(ids.size());

        assert(m_viewTask == nullptr);
        m_viewTask = m_tasksView->add(tr("Extracting data from new photos"));

        std::unique_lock storageLock(m_storageMutex);   // mark storage as being used so PhotosAnalyzerImpl::stop() will wait for its deletion

        // Construct DatabaseQueue as a shared pointer used to be passed to all tasks.
        // When last task is done, custom destructor below will do all necessary cleanups.
        storage = std::shared_ptr<Database::DatabaseQueue>(
            new Database::DatabaseQueue(m_database),
            [this, _ = std::move(storageLock)](Database::DatabaseQueue* queue)
        {
            // This lambda may be called from ~PhotosAnalyzerImpl as a result of stop().
            // What needs to be done immediately is to flush queue from all pending database task.
            // This is done by simply deleting queue which takes care of its tasks.
            //
            // Here we also delete progress view, but that needs to be postponed to main thread.
            // If called from ~PhotosAnalyzerImpl, then view should be already cleaned.
            delete queue;

            if (m_viewTask)
                QMetaObject::invokeMethod(this, &PhotosAnalyzerImpl::finishProgressBar);
        });
    }
    else
    {
        assert(m_viewTask != nullptr);
        m_maxPhotos += static_cast<int>(ids.size());
    }

    auto* progress = m_viewTask->getProgressBar();
    progress->setMaximum(m_maxPhotos);
    progress->setMinimum(0);

    for(const auto& id: ids)
        m_taskQueue.add(std::make_unique<UpdatePhoto>(storage, id, m_mediaInformation));
}


void PhotosAnalyzerImpl::stop()
{
    disconnect(m_backendConnection);
    m_taskQueue.clear();
    m_taskQueue.waitForPendingTasks();

    // make sure storage is unlocked
    std::lock_guard _(m_storageMutex);
}


void PhotosAnalyzerImpl::finishProgressBar()
{
    if (m_viewTask)
    {
        m_viewTask->finished();
        m_viewTask = nullptr;
    }
}



///////////////////////////////////////////////////////////////////////////////


PhotosAnalyzer::PhotosAnalyzer(ICoreFactoryAccessor* coreFactory,
                               Database::IDatabase& database)
    : m_data(new PhotosAnalyzerImpl(coreFactory, database))
{

}


PhotosAnalyzer::~PhotosAnalyzer()
{

}


void PhotosAnalyzer::set(ITasksView* tasksView)
{
    m_data->set(tasksView);
}
