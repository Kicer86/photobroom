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

#include <cassert>

#include <QPainter>

#include "images_tree_view.hpp"
#include "utils/variant_display.hpp"
#include <core/iphotos_manager.hpp>


TreeItemDelegate::TreeItemDelegate(QObject* p):
    QAbstractItemDelegate(p),
    m_backgroundEven(QColor(0, 0, 0, 0)),
    m_backgroundOdd(QColor(0, 0, 0, 0)),
    m_view(nullptr)
{

}


TreeItemDelegate::TreeItemDelegate(ImagesTreeView* view):
    QAbstractItemDelegate(view),
    m_backgroundEven(QColor(0, 0, 0, 0)),
    m_backgroundOdd(QColor(0, 0, 0, 0)),
    m_view(view)
{

}


TreeItemDelegate::~TreeItemDelegate()
{

}


void TreeItemDelegate::set(ImagesTreeView* view)
{
    m_view = view;
}


void TreeItemDelegate::setNodeBackgroundEvenColor(const QColor& c)
{
    m_backgroundEven = c;

    m_view->update();
}


void TreeItemDelegate::setNodeBackgroundOddColor(const QColor& c)
{
    m_backgroundOdd = c;

    m_view->update();
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
    const QRect& r = option.rect;
    QImage image = getImage(index, option.decorationSize);
    const QRect imageRect = image.rect();
    const int h_margin = (r.width()  - imageRect.width())  / 2;
    const int v_margin = (r.height() - imageRect.height()) / 2;

    if ( (option.state & QStyle::State_Enabled) == 0 )
        image = image.convertToFormat(QImage::Format_Grayscale8);

    painter->drawImage(r.x() + h_margin, r.y() + v_margin, image);
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
        const QColor brushColor = even? m_backgroundEven: m_backgroundOdd;
        QColor penColor = brushColor.darker();
        penColor.setAlpha(255);

        const QPen   pen(penColor, w);
        const QBrush brush(brushColor);

        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(overallRect);

        painter->restore();
    }

    // calculate title position
    const QAbstractItemModel* m = index.model();
    const QVariant v = m->data(index, Qt::DisplayRole);
    const QString t = VariantDisplay()(v, option.locale);
    const QString title = t.isEmpty()? tr("(Empty)"): t;

    // title bounding box
    const QRect& rect = option.rect;
    const QRect boundingRect = painter->boundingRect(rect, Qt::AlignCenter, title);

    const int margin = 5;
    const int y = rect.height()/2 + rect.top();
    const QLine left_horizontal_line(10, y, boundingRect.left() - margin, y);
    const QLine right_horizontal_line(boundingRect.right() + margin, y, rect.right() - 10, y);

    // draw top line
    painter->drawLine(left_horizontal_line);
    painter->drawLine(right_horizontal_line);

    // title
    painter->drawText(rect, Qt::AlignCenter, title);
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
