/*
 * A model proxy which adds extra row for data appending.
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "appendable_model_proxy.hpp"


AppendableModelProxy::AppendableModelProxy(QObject* p):
    QAbstractItemModel(p),
    m_sourceModel(nullptr)
{
}


AppendableModelProxy::~AppendableModelProxy()
{
}


int AppendableModelProxy::columnCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex sourceIdx = mapToSource(parent);
    const int cc = m_sourceModel->columnCount(sourceIdx);
    return std::min(cc, 2);
}


int AppendableModelProxy::rowCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex sourceIdx = mapToSource(parent);
    const int rc = m_sourceModel->rowCount(sourceIdx);
    return rc + 1;
}


QModelIndex AppendableModelProxy::parent(const QModelIndex& /*child*/) const
{
    return QModelIndex();      // only flat models supported - no parent for anyone
}


QModelIndex AppendableModelProxy::index(int row, int column, const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex sourceIdx = mapToSource(parent);
    const int rc = m_sourceModel->rowCount(sourceIdx);

    // if this is the last row (one row after last row of original model) then mark this index with `this`
    const void* id = row == rc? this: nullptr;
    void* fid = const_cast<void *>(id);

    const QModelIndex i = QAbstractItemModel::createIndex(row, column, fid);
    return i;
}


void AppendableModelProxy::setSourceModel(QAbstractItemModel* model)
{
    QAbstractItemModel* current = m_sourceModel;

    if (current)
        current->disconnect(this);

    m_sourceModel = model;

    if (model)
    {
        connect(model, &QAbstractItemModel::rowsAboutToBeInserted,
                this, &AppendableModelProxy::modelRowsAboutToBeInserted);

        connect(model, &QAbstractItemModel::rowsInserted,
                this, &AppendableModelProxy::modelRowsInserted);

        connect(model, &QAbstractItemModel::columnsAboutToBeInserted,
                this, &AppendableModelProxy::modelColumnsAboutToBeInserted);

        connect(model, &QAbstractItemModel::columnsInserted,
                this, &AppendableModelProxy::modelColumnsInserted);

        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &AppendableModelProxy::modelRowsAboutToBeRemoved);

        connect(model, &QAbstractItemModel::rowsRemoved,
                this, &AppendableModelProxy::modelRowsRemoved);

        connect(model, &QAbstractItemModel::modelAboutToBeReset,
                this, &AppendableModelProxy::sourceModelAboutToBeReset);

        connect(model, &QAbstractItemModel::modelReset,
                this, &AppendableModelProxy::sourceModelReset);
    }
}


QModelIndex AppendableModelProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
    const QModelIndex i = sourceIndex.isValid()?
        QAbstractItemModel::createIndex(sourceIndex.row(), sourceIndex.column()):
        QModelIndex();

    return i;
}


QModelIndex AppendableModelProxy::mapToSource(const QModelIndex& proxyIndex) const
{
    // if internalPointer is set, then `proxyIndex` is part of extra row
    const QModelIndex i = proxyIndex.isValid() && proxyIndex.internalPointer() == nullptr?
        m_sourceModel->index(proxyIndex.row(), proxyIndex.column()):
        QModelIndex();

    return i;
}


Qt::ItemFlags AppendableModelProxy::flags(const QModelIndex& index) const
{
    const auto f = index.isValid() && index.internalPointer() == this?
        ( Qt::ItemIsEnabled |
          Qt::ItemIsDropEnabled |
          Qt::ItemIsDragEnabled |
          Qt::ItemIsEditable |
          Qt::ItemIsSelectable   ):
        m_sourceModel->flags(mapToSource(index));

    return f;
}


QVariant AppendableModelProxy::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid() && index.internalPointer() != this)
        result = m_sourceModel->data(mapToSource(index), role);

    return result;
}


QVariant AppendableModelProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
    return m_sourceModel->headerData(section, orientation, role);
}


void AppendableModelProxy::modelRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::beginInsertRows(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelRowsInserted(const QModelIndex &parent, int /*first*/, int /*last*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::endInsertRows();
}


void AppendableModelProxy::modelColumnsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::beginInsertColumns(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelColumnsInserted(const QModelIndex& parent, int /*first*/, int /*last*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::endInsertColumns();
}


void AppendableModelProxy::modelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::beginRemoveRows(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelRowsRemoved(const QModelIndex& parent, int /*start*/, int /*end*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::endRemoveRows();
}


void AppendableModelProxy::sourceModelAboutToBeReset()
{
    QAbstractItemModel::beginResetModel();
}


void AppendableModelProxy::sourceModelReset()
{
    QAbstractItemModel::endResetModel();
}
