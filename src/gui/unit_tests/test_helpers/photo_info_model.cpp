
#include "photo_info_model.hpp"


PhotoInfoModel::PhotoInfoModel(QAbstractItemModel* m, QObject* p): APhotoInfoModel(p), m_model(m)
{
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &PhotoInfoModel::rowsInserted);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &PhotoInfoModel::rowsRemoved);
    connect(m_model, &QAbstractItemModel::rowsMoved, this, &PhotoInfoModel::rowsMoved);
    connect(m_model, &QAbstractItemModel::modelReset, this, &PhotoInfoModel::modelReset);
}


PhotoInfoModel::~PhotoInfoModel()
{

}


QVariant PhotoInfoModel::data(const QModelIndex& _index, int role) const
{
    return m_model->data(_index, role);
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


MockPhotoInfoModel::MockPhotoInfoModel(QAbstractItemModel* m, QObject* p): PhotoInfoModel(m, p)
{

}

