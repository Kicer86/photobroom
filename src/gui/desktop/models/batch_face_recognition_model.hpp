
#ifndef BATCH_FACE_RECOGNITION_MODEL_HPP_INCLUDED
#define BATCH_FACE_RECOGNITION_MODEL_HPP_INCLUDED

#include <QAbstractListModel>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>


class BatchFaceRecognitionModel: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Database::IDatabase* database WRITE setDatabase READ database REQUIRED)
    Q_PROPERTY(ICoreFactoryAccessor* core MEMBER m_core REQUIRED)

public:
    BatchFaceRecognitionModel();

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void setDatabase(Database::IDatabase *);
    Database::IDatabase* database();

private:
    Database::IDatabase* m_db;
    ICoreFactoryAccessor* m_core;

    void beginAnalysis();
};

#endif
