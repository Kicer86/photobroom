
#ifndef BATCH_FACE_DETECTOR_HPP_INCLUDED
#define BATCH_FACE_DETECTOR_HPP_INCLUDED

#include <deque>
#include <QObject>

#include <database/idatabase.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ilogger.hpp>
#include <core/itask_executor.hpp>

#include "models/aphoto_data_model.hpp"
#include "people_editor.hpp"


class BatchFaceDetector: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ICoreFactoryAccessor* core WRITE setCore READ core REQUIRED)
    Q_PROPERTY(Database::IDatabase* db WRITE setDB READ db REQUIRED)
    Q_PROPERTY(APhotoDataModel* photos_model WRITE setPhotosModel READ photosModel)

public:
    ~BatchFaceDetector();

    void setPhotosModel(APhotoDataModel *);
    void setCore(ICoreFactoryAccessor *);
    void setDB(Database::IDatabase *);

    APhotoDataModel* photosModel() const;
    ICoreFactoryAccessor* core() const;
    Database::IDatabase* db() const;

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &, int) const override;

private:
    struct Face
    {
        Face(std::unique_ptr<IFace> f, const QImage& img)
            : faceData(std::move(f))
            , faceImg(img)
        {}

        Face(const Face &) = delete;
        Face(Face &&) = default;

        Face& operator=(const Face &) = delete;
        Face& operator=(Face &&) = default;

        std::unique_ptr<IFace> faceData;
        QImage faceImg;
    };

    std::deque<Photo::Id> m_ids;
    std::mutex m_idsMtx;
    std::vector<Face> m_faces;
    std::unique_ptr<ILogger> m_logger;
    APhotoDataModel* m_photosModel = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;
    Database::IDatabase* m_db = nullptr;
    std::shared_ptr<ITaskExecutor::IProcessControl> m_photosProcessingProcess;

    ITaskExecutor::ProcessCoroutine processPhotos(ITaskExecutor::IProcessSupervisor *);
    void appendFaces(std::vector<Face> &&);
    void newPhotos(const QModelIndex &, int, int);
};

#endif
