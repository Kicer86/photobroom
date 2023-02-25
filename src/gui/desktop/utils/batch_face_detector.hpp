
#ifndef BATCH_FACE_DETECTOR_HPP_INCLUDED
#define BATCH_FACE_DETECTOR_HPP_INCLUDED

#include <QObject>

#include <core/icore_factory_accessor.hpp>
#include "models/aphoto_data_model.hpp"



class BatchFaceDetector: public QObject
{
    Q_OBJECT
    Q_PROPERTY(ICoreFactoryAccessor* core WRITE setCore READ core REQUIRED)
    Q_PROPERTY(APhotoDataModel* photos_model WRITE setPhotosModel READ photosModel)

public:
    void setPhotosModel(APhotoDataModel *);
    void setCore(ICoreFactoryAccessor *);

    APhotoDataModel* photosModel() const;
    ICoreFactoryAccessor* core() const;

private:
    APhotoDataModel* m_photosModel = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;

    // thread eating photos from model
    void processPhotos();
};

#endif
