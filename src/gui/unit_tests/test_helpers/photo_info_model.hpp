
#ifndef PHOTO_INFO_MODEL_HPP
#define PHOTO_INFO_MODEL_HPP

#include "models/aphoto_info_model.hpp"

struct IPhotoInfo;

class PhotoInfoModel: public APhotoInfoModel
{
    public:
        PhotoInfoModel(QAbstractItemModel *, IPhotoInfo *, QObject* = 0);
        virtual ~PhotoInfoModel();

        // QAbstractItemModel:
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        virtual QModelIndex parent(const QModelIndex& child) const override;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        // APhotoInfoModel:
        virtual IPhotoInfo* getPhotoInfo(const QModelIndex&) const override;

    private:
        QAbstractItemModel* m_model;
        IPhotoInfo* m_photoInfo;
};

#endif
