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

#include <QCache>
#include <QPainter>

#include "view_helpers/data.hpp"

TreeItemDelegate::TreeItemDelegate(): QAbstractItemDelegate(), m_rotationData(new QCache<QModelIndex, RotationData>(1000))
{

}


TreeItemDelegate::~TreeItemDelegate()
{

}


QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize();
}


void TreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QRect& r = option.rect;

    if ( (option.state & QStyle::State_Selected) != 0)
    {
        auto oldPen = painter->pen();
        auto oldBrush = painter->brush();

        painter->setPen(option.palette.color(QPalette::Highlight));
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        painter->drawRect(r);

        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }

    const bool image = (option.features & QStyleOptionViewItem::HasDecoration) != 0;

    if (image)
        paintImage(painter, option, index);
    else
        paintNode(painter, option, index);
}


void TreeItemDelegate::paintImage(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QAbstractItemModel* m = index.model();
    const QRect& r = option.rect;
    const QVariant v = m->data(index, Qt::DecorationRole);
    const QPixmap p = v.value<QPixmap>();
    const int rot = getRotationFor(index);
    const QPixmap rotatedPixmap = p.transformed(QMatrix().rotate(rot), Qt::SmoothTransformation);
    const int h_margin = (r.width()  - p.rect().width()) / 2;
    const int v_margin = (r.height() - p.rect().height()) / 2;

    painter->drawPixmap(r.x() + h_margin, r.y() + v_margin, rotatedPixmap);
}


void TreeItemDelegate::paintNode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QAbstractItemModel* m = index.model();
    const QRect& r = option.rect;
    const QVariant v = m->data(index, Qt::DisplayRole);
    const QString t = v.toString();

    painter->drawText(r, Qt::AlignCenter, t);
}


int TreeItemDelegate::getRotationFor(const QModelIndex& idx) const
{
    RotationData* data = m_rotationData->object(idx);

    if (data == nullptr)
    {
        data = new RotationData;
        data->rotation = rand() % 11 - 5;
        m_rotationData->insert(idx, data);
    }

    return data->rotation;
}
