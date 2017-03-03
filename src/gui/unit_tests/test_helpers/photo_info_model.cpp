
#include "photo_info_model.hpp"


PhotoInfoModel::PhotoInfoModel(QAbstractItemModel* m, QObject* p): APhotoInfoModel(p), m_model(m)
{
    connect(m_model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(rowsInserted(QModelIndex, int, int)));
    connect(m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SIGNAL(rowsRemoved(QModelIndex, int, int)));
    connect(m_model, SIGNAL(rowsMoved(QModelIndex,int, int, QModelIndex, int)),
            this, SIGNAL(rowsMoved(QModelIndex,int, int, QModelIndex, int)));

    connect(m_model, SIGNAL(modelReset()), this, SIGNAL(modelReset()));


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

