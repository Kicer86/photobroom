
#include <database/filter.hpp>
#include <database/iphoto_operator.hpp>

#include "batch_face_recognition_model.hpp"


BatchFaceRecognitionModel::BatchFaceRecognitionModel()
{
    // perform initialization when all required properties are set
    QMetaObject::invokeMethod(this, [this]
    {
        assert(m_db != nullptr);
        assert(m_core != nullptr);

        beginAnalysis();
    },
    Qt::QueuedConnection);
}


int BatchFaceRecognitionModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}


QVariant BatchFaceRecognitionModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    return result;
}


void BatchFaceRecognitionModel::setDatabase(Database::IDatabase* db)
{
    m_db = db;
}


Database::IDatabase* BatchFaceRecognitionModel::database()
{
    return m_db;
}


void BatchFaceRecognitionModel::beginAnalysis()
{
    m_db->exec([](Database::IBackend& backend)
    {
        auto baseFitler = Database::getValidPhotosFilter();
        backend.photoOperator().getPhotos();
    });
}
