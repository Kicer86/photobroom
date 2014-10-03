/*
 * Default delegate for ImageTreeView
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

#include "tree_item_delegate.hpp"

#include <QPainter>

#include "view_helpers/data.hpp"

TreeItemDelegate::TreeItemDelegate(): m_data(nullptr)
{

}


TreeItemDelegate::~TreeItemDelegate()
{

}


void TreeItemDelegate::set(Data* data)
{
    m_data = data;
}


QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{

}


void TreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QAbstractItemModel* m = index.model();
    const ModelIndexInfo& info = m_data->get(index);
    const QRect& r = info.getRect();
    const bool image = m_data->isImage(index);

    if (image)
    {
        const QVariant v = m->data(index, Qt::DecorationRole);
        const QPixmap p = v.value<QPixmap>();

        painter->drawPixmap(r.x() + m_data->indexMargin, r.y() + m_data->indexMargin, p);
    }
    else
    {
        const QVariant v = m->data(index, Qt::DisplayRole);
        const QString t = v.toString();

        painter->drawText(r, Qt::AlignCenter, t);
    }
}
