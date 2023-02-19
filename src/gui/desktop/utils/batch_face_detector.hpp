
#ifndef BATCH_FACE_DETECTOR_HPP_INCLUDED
#define BATCH_FACE_DETECTOR_HPP_INCLUDED

#include <QObject>

#include "models/aphoto_data_model.hpp"


class BatchFaceDetector: public QObject
{
    Q_OBJECT
    Q_PROPERTY(APhotoDataModel* photos_model WRITE setPhotosModel READ photosModel)

public:
    void setPhotosModel(APhotoDataModel *);
    APhotoDataModel* photosModel() const;

private:
    APhotoDataModel* m_photosModel;
};

#endif
