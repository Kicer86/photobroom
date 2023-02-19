
#include "batch_face_detector.hpp"


void BatchFaceDetector::setPhotosModel(APhotoDataModel* model)
{
    m_photosModel = model;
}


APhotoDataModel *BatchFaceDetector::photosModel() const
{
    return m_photosModel;
}
