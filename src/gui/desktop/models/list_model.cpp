/*
 * Flat list model with lazy image load.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "list_model.hpp"
#include "list_model_p.hpp"

#include <QFileInfo>


ListModelPrivate::ListModelPrivate(ListModel* q): q(q), m_data()
{
}


ListModelPrivate::~ListModelPrivate()
{
}


///////////////////////////////////////////////////////////////////////////////


ListModel::ListModel(QObject* p):
    QAbstractItemModel(p),
    d(new ListModelPrivate(this))
{

}


ListModel::ListModel(const ListModel& other)
    : d(new ListModelPrivate(this))
{

}


ListModel::~ListModel()
{
    delete d;
}


ListModel& ListModel::operator=(const ListModel& other)
{

}


bool ListModel::operator==(const ListModel& other) const
{

}


void ListModel::insert(const QString& path)
{
    const int s = d->m_data.size();

    beginInsertRows(QModelIndex(), s, s);

    d->m_data.push_back(path);

    endInsertRows();
}


void ListModel::clear()
{
    beginResetModel();

    d->m_data.clear();

    endResetModel();
}


QVariant ListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.isValid())
    {
        Info& info = d->m_data[index.row()];

        switch(role)
        {
            case Qt::DisplayRole:
            {
                if (info.filename.isEmpty())
                {
                    QFileInfo file_info(info.path);
                    info.filename = file_info.fileName();
                }

                result = info.filename;
                break;
            }

            case Qt::DecorationRole:
            {
                if (info.pixmap.isNull())
                {
                    QPixmap pixmap(info.path);
                    info.pixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }

                result = info.pixmap;
            }

            default:
                break;
        }
    }

    return result;
}


int ListModel::columnCount(const QModelIndex& parent) const
{
    const int result = parent.isValid()? 0 : 1;

    return result;
}


int ListModel::rowCount(const QModelIndex& parent) const
{
    const int result = parent.isValid()? 0: d->m_data.size();

    return result;
}


QModelIndex ListModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}


QModelIndex ListModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, nullptr);
}
