
#include "photo_info_model.hpp"

PhotoInfoModel::PhotoInfoModel(QAbstractItemModel* m, QObject* p): APhotoInfoModel(p), m_model(m)
{

}


PhotoInfoModel::~PhotoInfoModel()
{

}


int PhotoInfoModel::columnCount(const QModelIndex& parent) const
{
    return m_model->columnCount(parent);
}


QModelIndex PhotoInfoModel::index(int row, int column, const QModelIndex& parent) const
{
    return m_model->index(row, column, parent);
}


QModelIndex PhotoInfoModel::parent(const QModelIndex& child) const
{
    return m_model->parent(child);
}


int PhotoInfoModel::rowCount(const QModelIndex& parent) const
{
    return m_model->rowCount(parent);
}


IPhotoInfo* PhotoInfoModel::getPhotoInfo(const QModelIndex&) const
{
    return nullptr;
}
