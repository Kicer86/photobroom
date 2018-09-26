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
    QAbstractProxyModel(p)
{
}


AppendableModelProxy::~AppendableModelProxy()
{
}


int AppendableModelProxy::columnCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex sourceIdx = mapToSource(parent);
    const int cc = sourceModel()->columnCount(sourceIdx);
    return cc;
}


int AppendableModelProxy::rowCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex sourceIdx = mapToSource(parent);
    const int rc = sourceModel()->rowCount(sourceIdx);
    return rc;
}


QModelIndex AppendableModelProxy::parent(const QModelIndex& child) const
{
    const QModelIndex sourceIdx = mapToSource(child);
    const QModelIndex sourceParent = sourceModel()->parent(sourceIdx);
    const QModelIndex p = mapFromSource(sourceParent);
;
    assert(p.isValid() == false);     // only flat models are supported

    return p;
}


QModelIndex AppendableModelProxy::index(int row, int column, const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    const QModelIndex i = QAbstractProxyModel::createIndex(row, column);
    return i;
}


void AppendableModelProxy::setSourceModel(QAbstractItemModel* model)
{
    QAbstractItemModel* current = sourceModel();

    if (current)
        current->disconnect(this);

    QAbstractProxyModel::setSourceModel(model);

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
        QAbstractProxyModel::createIndex(sourceIndex.row(), sourceIndex.column()):
        QModelIndex();

    return i;
}


QModelIndex AppendableModelProxy::mapToSource(const QModelIndex& proxyIndex) const
{
    const QModelIndex i = proxyIndex.isValid()?
        sourceModel()->index(proxyIndex.row(), proxyIndex.column()):
        QModelIndex();

    return i;
}


void AppendableModelProxy::modelRowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::beginInsertRows(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelRowsInserted(const QModelIndex &parent, int /*first*/, int /*last*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::endInsertRows();
}


void AppendableModelProxy::modelColumnsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::beginInsertColumns(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelColumnsInserted(const QModelIndex& parent, int /*first*/, int /*last*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::endInsertColumns();
}


void AppendableModelProxy::modelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::beginRemoveRows(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelRowsRemoved(const QModelIndex& parent, int /*start*/, int /*end*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractProxyModel::endRemoveRows();
}


void AppendableModelProxy::sourceModelAboutToBeReset()
{
    QAbstractItemModel::beginResetModel();
}


void AppendableModelProxy::sourceModelReset()
{
    QAbstractItemModel::endResetModel();
}
