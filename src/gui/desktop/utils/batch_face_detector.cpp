
#include <core/exif_reader_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/logger_factory.hpp>
#include <core/oriented_image.hpp>
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>

#include "batch_face_detector.hpp"

#include <iostream>

BatchFaceDetector::~BatchFaceDetector()
{

}


void BatchFaceDetector::setPhotosModel(APhotoDataModel* model)
{
    if (m_photosModel != nullptr)
    {
        disconnect(m_photosModel, &QAbstractItemModel::rowsInserted, this, &BatchFaceDetector::newPhotos);
    }

    m_photosModel = model;

    if (m_photosModel != nullptr)
    {
        assert(m_core != nullptr);

        connect(m_photosModel, &QAbstractItemModel::rowsInserted, this, &BatchFaceDetector::newPhotos);

        QMetaObject::invokeMethod(this, [this]
        {
            const auto rows = m_photosModel->rowCount();
            m_faces.clear();
            m_ids.clear();

            if (rows > 0)
                newPhotos({}, 0, rows - 1);
        },
        Qt::QueuedConnection);
    }
}


void BatchFaceDetector::setCore(ICoreFactoryAccessor* core)
{
    assert(m_core == nullptr);
    m_core = core;
    m_logger = m_core->getLoggerFactory().get("BatchFaceDetector");

    m_core->getTaskExecutor().add([]() -> ITaskExecutor::ProcessCoroutine
    {
        while(true)
        {
            co_await std::suspend_always{};

            std::cout << "5\n";
        }
    });
}


void BatchFaceDetector::setDB(Database::IDatabase* db)
{
    m_db = db;
}


APhotoDataModel* BatchFaceDetector::photosModel() const
{
    return m_photosModel;
}


ICoreFactoryAccessor* BatchFaceDetector::core() const
{
    return m_core;
}


Database::IDatabase* BatchFaceDetector::db() const
{
    return m_db;
}


int BatchFaceDetector::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: static_cast<int>(m_faces.size());
}


QVariant BatchFaceDetector::data(const QModelIndex& idx, int role) const
{
    assert(idx.row() < static_cast<int>(m_faces.size()));
    assert(idx.column() == 0);

    const size_t row = static_cast<size_t>(idx.row());

    if (role == Qt::DisplayRole)
        return m_faces[row].faceData->name();
    else if (role == Qt::DecorationRole)
        return m_faces[row].faceImg;
    else
        return {};
}


void BatchFaceDetector::kickProcessing()
{
    if (m_ids.empty() == false)
        QMetaObject::invokeMethod(this, &BatchFaceDetector::processPhotos, Qt::QueuedConnection);
}


void BatchFaceDetector::processPhotos()
{
    assert(m_ids.empty() == false);
    QPointer modelPtr(m_photosModel);

    FaceEditor fe(*m_db, *m_core, m_logger);

    const auto id = m_ids.front();
    m_ids.pop_front();

    runOn(m_core->getTaskExecutor(), [fe = std::move(fe), id, this]() mutable
    {
        auto faces = fe.getFacesFor(id);
        std::vector<Face> facesDetails;

        for (auto& face: faces)
        {
            const auto faceImg = face->image()->copy(face->rect());
            facesDetails.emplace_back(std::move(face), faceImg);
        }

        invokeMethod(this, &BatchFaceDetector::appendFaces, std::move(facesDetails));
    });
}


void BatchFaceDetector::appendFaces(std::vector<Face>&& faces)
{
    if (faces.empty() == false)
    {
        const int newFaces = static_cast<int>(faces.size());
        const int existingFaces = static_cast<int>(m_faces.size());
        beginInsertRows({}, existingFaces, existingFaces + newFaces - 1);
        m_faces.insert(m_faces.end(), std::make_move_iterator(faces.begin()),  std::make_move_iterator(faces.end()));
        endInsertRows();
    }

    kickProcessing();
}


void BatchFaceDetector::newPhotos(const QModelIndex &, int first, int last)
{
    const bool needKicking = m_ids.empty();

    for(int row = first; row <= last; row++)
    {
        const Photo::Id id(m_photosModel->data(m_photosModel->index(row, 0), APhotoDataModel::PhotoIdRole));
        m_ids.push_back(id);
    }

    if (needKicking)
        kickProcessing();
}
