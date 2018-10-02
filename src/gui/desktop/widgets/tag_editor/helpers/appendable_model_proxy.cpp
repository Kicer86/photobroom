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


AppendableModelProxy::AppendableModelProxy(int defCC, QObject* p):
    QAbstractItemModel(p),
    m_sourceModel(nullptr),
    m_defCC(defCC),
    m_rows(0),
    m_cols(0)
{
    updateRowData();
}


AppendableModelProxy::~AppendableModelProxy()
{
}


int AppendableModelProxy::columnCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    return m_cols;
}


int AppendableModelProxy::rowCount(const QModelIndex& parent) const
{
    assert(parent.isValid() == false);     // only flat models are supported

    return m_rows;
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


bool AppendableModelProxy::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool result = true;

    if (index.isValid() && index.internalPointer() == this)
    {
        assert( static_cast<int>(m_lastRowData.size()) > index.column());
        auto& col_data = m_lastRowData[index.column()];
        col_data[role] = value;
    }
    else
        result = m_sourceModel->setData(mapToSource(index), value, role);

    return result;
}


void AppendableModelProxy::setSourceModel(QAbstractItemModel* model)
{
    m_cols = 0;
    m_rows = 0;

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

        // setup initial counts
        setupCount();
    }

    updateRowData();
}


void AppendableModelProxy::updateRowData()
{
    // make sure we have enought space for last row's data
    m_lastRowData.resize(m_cols);
}


void AppendableModelProxy::setupCount()
{
    m_rows = m_sourceModel->rowCount() + 1;
    m_cols = m_sourceModel->columnCount();
    m_cols = m_rows == 1 && m_cols == 0? m_defCC: m_cols;   // just one row (ours)? and no columns - use default columns count
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

    if (index.isValid() && index.internalPointer() == this)
    {
        assert(static_cast<int>(m_lastRowData.size()) > index.column());
        auto& col_data = m_lastRowData[index.column()];
        auto it = col_data.find(role);

        if (it != col_data.end())
            result = it->second;
    }
    else
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


void AppendableModelProxy::modelRowsInserted(const QModelIndex &parent, int first, int last)
{
    assert(parent.isValid() == false);     // only flat models are supported

    m_rows += last - first + 1;

    assert(m_sourceModel->rowCount() + 1 == m_rows);

    QAbstractItemModel::endInsertRows();
}


void AppendableModelProxy::modelColumnsAboutToBeInserted(const QModelIndex& parent, int start, int end)
{
    // check current size (columns) of source model
    // it is possible to have a significant dismatch here -
    // if source model was empty, AppendableModelProxy
    // has created some fake columns (as defined in constructor).
    // Now when source model is being filled, we need to adjust.

    const int count = end - start + 1;
    const int sourceColumns = m_sourceModel->columnCount(parent) + count;
    const int diff = sourceColumns - m_cols;

    if (diff < 0)
    {
        // not tested! I guess there is some math error
        const int last = m_cols - 1;
        const int first = last + diff;

        assert(!"not tested");
        QAbstractItemModel::beginRemoveColumns(mapFromSource(parent), first, last);

        m_postColumnInsertAction = std::bind(&AppendableModelProxy::endRemoveColumns, this);
    }
    else if (diff == 0)
    {
        // nothing to do
        m_postColumnInsertAction = []{};
    }
    else
    {
        // not tested! I guess there is some math error
        const int first = m_cols;
        const int last = first + diff;

        assert(!"not tested");
        QAbstractItemModel::beginInsertColumns(mapFromSource(parent), first, last);

        m_postColumnInsertAction = std::bind(&AppendableModelProxy::endRemoveColumns, this);
    }
}


void AppendableModelProxy::modelColumnsInserted(const QModelIndex& parent, int /*first*/, int /*last*/)
{
    assert(parent.isValid() == false);     // only flat models are supported

    m_cols = m_sourceModel->columnCount(parent);
    updateRowData();

    m_postColumnInsertAction();
    m_postColumnInsertAction = std::function<void()>();
}


void AppendableModelProxy::modelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    QAbstractItemModel::beginRemoveRows(mapFromSource(parent), start, end);
}


void AppendableModelProxy::modelRowsRemoved(const QModelIndex& parent, int start, int end)
{
    assert(parent.isValid() == false);     // only flat models are supported

    m_rows -= end - start + 1;

    assert(m_sourceModel->rowCount() + 1 == m_rows);

    QAbstractItemModel::endRemoveRows();
}


void AppendableModelProxy::sourceModelAboutToBeReset()
{
    QAbstractItemModel::beginResetModel();
}


void AppendableModelProxy::sourceModelReset()
{
    setupCount();
    updateRowData();
    QAbstractItemModel::endResetModel();
}
