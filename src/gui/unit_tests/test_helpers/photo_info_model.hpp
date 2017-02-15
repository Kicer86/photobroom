
#ifndef PHOTO_INFO_MODEL_HPP
#define PHOTO_INFO_MODEL_HPP

#include <gmock/gmock.h>

#include "models/aphoto_info_model.hpp"

struct IPhotoInfo;

class PhotoInfoModel: public APhotoInfoModel
{
    public:
        PhotoInfoModel(QAbstractItemModel *, QObject* = 0);
        PhotoInfoModel(const PhotoInfoModel &) = delete;
        virtual ~PhotoInfoModel();

        PhotoInfoModel& operator=(const PhotoInfoModel &) = delete;

        // QAbstractItemModel:
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& child) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    private:
        QAbstractItemModel* m_model;
};


struct MockPhotoInfoModel: PhotoInfoModel
{
    MockPhotoInfoModel(QAbstractItemModel*, QObject* = 0);

    MOCK_CONST_METHOD2(data, QVariant(const QModelIndex &, int));
    MOCK_CONST_METHOD1(getPhotoDetails, APhotoInfoModel::PhotoDetails(const QModelIndex &));
};


#endif
