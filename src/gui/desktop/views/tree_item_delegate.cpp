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

#include "images_tree_view.hpp"
#include "utils/variant_display.hpp"


TreeItemDelegate::TreeItemDelegate(QObject* p): QAbstractItemDelegate(p), m_view(nullptr)
{

}


TreeItemDelegate::TreeItemDelegate(ImagesTreeView* view): QAbstractItemDelegate(view), m_view(view)
{

}


TreeItemDelegate::~TreeItemDelegate()
{

}


void TreeItemDelegate::set(ImagesTreeView* view)
{
    m_view = view;
}


QSize TreeItemDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const
{
    return QSize();
}


void TreeItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QRect r = option.rect;
    r.adjust(0, 0, -1, -1);

    const bool image = (option.features & QStyleOptionViewItem::HasDecoration) != 0;

    if ( image && (option.state & QStyle::State_Selected) != 0)
    {
        auto oldPen = painter->pen();
        auto oldBrush = painter->brush();

        painter->setPen(option.palette.color(QPalette::Highlight));
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        painter->drawRect(r);

        painter->setPen(oldPen);
        painter->setBrush(oldBrush);
    }

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
    const QPixmap p = getPixmap(option, v);
    const int h_margin = (r.width()  - p.rect().width())  / 2;
    const int v_margin = (r.height() - p.rect().height()) / 2;

    painter->drawPixmap(r.x() + h_margin, r.y() + v_margin, p);
}


void TreeItemDelegate::paintNode(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // draw overall rect
    if (option.state & QStyle::State_Open && m_view != nullptr)
    {
        painter->save();

        const int w = 3;

        //combine start point with overall size to bound all child items
        QRect overallRect = m_view->childrenSize(index);
        overallRect.adjust(w, w, -w, -w);

        const bool even = index.row() % 2 == 0;
        const int r = even? 255 : 0;
        const int g = 0;
        const int b = even? 0: 255;

        const QPen   pen(QColor(r/2, g/2, b/2), w);
        const QBrush brush(QColor(r, g, b, 64));

        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(overallRect);

        painter->restore();
    }

    // calculate title position
    const QAbstractItemModel* m = index.model();
    const QVariant v = m->data(index, Qt::DisplayRole);
    const QString t = VariantDisplay()(v, option.locale);

    // title bounding box
    const QRect& rect = option.rect;
    const QRect boundingRect = painter->boundingRect(rect, Qt::AlignCenter, t);

    const int margin = 5;
    const int y = rect.height()/2 + rect.top();
    const QLine left_horizontal_line(10, y, boundingRect.left() - margin, y);
    const QLine right_horizontal_line(boundingRect.right() + margin, y, rect.right() - 10, y);

    // draw top line
    painter->drawLine(left_horizontal_line);
    painter->drawLine(right_horizontal_line);

    // title
    painter->drawText(rect, Qt::AlignCenter, t);
}


QIcon::Mode TreeItemDelegate::iconMode(const QStyle::State& state) const
{
    QIcon::Mode result;

    if ( (state & QStyle::State_Enabled) == false)
        result = QIcon::Disabled;
    if ( (state & QStyle::State_Selected) == true)
        result = QIcon::Selected;
    else
        result = QIcon::Normal;

    return result;
}


QIcon::State TreeItemDelegate::iconState(const QStyle::State& state) const
{
    return state & QStyle::State_Open ? QIcon::On : QIcon::Off;
}


QPixmap TreeItemDelegate::getPixmap(const QStyleOptionViewItem& option, const QVariant& variant) const
{
    QPixmap result;

    switch (variant.type())
    {
        case QVariant::Icon:
        {
            const QIcon::Mode mode = iconMode(option.state);
            const QIcon::State state = iconState(option.state);
            result = qvariant_cast<QIcon>(variant).pixmap(option.decorationSize, mode, state);
            break;
        }

        case QVariant::Color:
        {
            result = QPixmap(option.decorationSize);
            result.fill(qvariant_cast<QColor>(variant));
            break;
        }

        default:
            result = qvariant_cast<QPixmap>(variant);
            break;
    }

    return result;
}
