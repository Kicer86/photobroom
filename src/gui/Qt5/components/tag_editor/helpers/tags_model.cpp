/*
 * Tags model
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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
 *
 */

/// TODO: remove
#if defined _MSC_VER
    #if _MSC_VER >= 1800
        #define Q_COMPILER_INITIALIZER_LISTS
    #else
        #error unsupported compiler
    #endif
#endif


#include "tags_model.hpp"

#include <QItemSelectionModel>

#include "model_view/db_data_model.hpp"

TagsModel::TagsModel(QObject* p):
    QStandardItemModel(p),
    m_selectionModel(nullptr),
    m_dbDataModel(nullptr)
{

}


TagsModel::~TagsModel()
{

}


void TagsModel::set(QItemSelectionModel* selectionModel)
{
    if (m_selectionModel != nullptr)
        m_selectionModel->disconnect(this);

    m_selectionModel = selectionModel;
    connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(refreshModel(QItemSelection, const QItemSelection &)));

    refreshModel();
}


void TagsModel::set(DBDataModel* dbDataModel)
{
    m_dbDataModel = dbDataModel;
}


void TagsModel::refreshModel()
{
    if (m_dbDataModel != nullptr && m_selectionModel != nullptr)
    {
        clearModel();

        QItemSelection selection = m_selectionModel->selection();

        for (const QItemSelectionRange& range : selection)
        {
            QModelIndexList idxList = range.indexes();

            for (const QModelIndex& idx : idxList)
                addItem(idx);
        }
    }
}


void TagsModel::clearModel()
{
    clear();
}


void TagsModel::addItem(const QModelIndex& idx)
{
    IPhotoInfo::Ptr photo = m_dbDataModel->getPhoto(idx);
    const Tag::TagsList& tags = photo->getTags();

    for (const auto& tag: tags)
    {
        Tag::Info info(tag);
        QStandardItem* name = new QStandardItem(info.name());
        QStandardItem* value = new QStandardItem(info.valuesString());

        const QList<QStandardItem *> items( { name, value });
        appendRow(items);
    }
}


void TagsModel::refreshModel(const QItemSelection &, const QItemSelection &)
{
    refreshModel();
}
