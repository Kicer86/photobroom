
#include <core/exif_reader_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/logger_factory.hpp>
#include <core/oriented_image.hpp>
#include <core/task_executor_utils.hpp>

#include "batch_face_detector.hpp"

#include <iostream>


BatchFaceDetector::BatchFaceDetector()
    : m_faceEditor(make_lazy_ptr<FaceEditor>([this]{ return std::make_unique<FaceEditor>(*this->db(), *this->core(), this->m_logger); }))
{

}


BatchFaceDetector::~BatchFaceDetector()
{
    // db client should be destroyed by now
    assert(m_dbClient.get() == nullptr);
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
}


void BatchFaceDetector::setDB(Database::IDatabase* db)
{
    m_dbClient = db->attach(tr("Batch face detector"));
    if (m_dbClient)
    {
        m_dbClient->onClose([this]()
        {
            m_photosProcessingProcess->terminate();
        });

        // begin photo analysis
        auto process = std::bind(&BatchFaceDetector::processPhotos, this, std::placeholders::_1);
        m_photosProcessingProcess = m_core->getTaskExecutor().add(process);
    }
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
    return m_dbClient? &m_dbClient->db(): nullptr;
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


bool BatchFaceDetector::setData(const QModelIndex& idx, const QVariant& value, int role)
{
    assert(idx.row() < static_cast<int>(m_faces.size()));
    assert(idx.column() == 0);
    const QString name = value.toString();
    const size_t row = static_cast<size_t>(idx.row());

    auto& face = m_faces[row].faceData;

    if (role == Qt::EditRole && name != face->name())
    {
        face->setName(name);

        return true;
    }
    else
        return false;
}


void BatchFaceDetector::accept(int idx)
{

}


void BatchFaceDetector::drop(int idx)
{

}


ITaskExecutor::ProcessCoroutine BatchFaceDetector::processPhotos(ITaskExecutor::IProcessSupervisor* supervisor)
{
    while(supervisor->keepWorking())
    {
        const auto id_opt = getNextId();

        if (id_opt)
        {
            const auto id = *id_opt;

            // no data in db, generate
            runOn(m_core->getTaskExecutor(), [id, this, supervisor]() mutable
            {
                loadFacesFromPhoto(id);
                supervisor->resume();                                   // restore this task after faces were loaded from photo
            });
        }

        co_yield ITaskExecutor::ProcessState::Suspended;
    }

    // face scanning is done, db won't be needed anymore, release it
    m_dbClient.reset();
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
}


void BatchFaceDetector::newPhotos(const QModelIndex &, int first, int last)
{
    std::lock_guard _(m_idsMtx);

    for(int row = first; row <= last; row++)
    {
        const Photo::Id id(m_photosModel->data(m_photosModel->index(row, 0), APhotoDataModel::PhotoIdRole));
        m_ids.push_back(id);
    }

    // make sure processing process is not suspended
    m_photosProcessingProcess->resume();
}


std::optional<Photo::Id> BatchFaceDetector::getNextId()
{
    std::lock_guard lk(m_idsMtx);

    std::optional<Photo::Id> result;

    if (m_ids.empty() == false)
    {
        result = m_ids.front();
        m_ids.pop_front();
    }

    return result;
}


void BatchFaceDetector::loadFacesFromPhoto(const Photo::Id& id)
{
    auto faces = m_faceEditor->getFacesFor(id);
    std::vector<Face> facesDetails;

    // prepare details for model
    for (auto& face: faces)
    {
        const auto faceImg = face->image()->copy(face->rect());
        facesDetails.emplace_back(std::move(face), faceImg);
    }

    invokeMethod(this, &BatchFaceDetector::appendFaces, std::move(facesDetails));
}
