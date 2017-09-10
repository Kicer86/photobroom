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
    assert(option.rect.size().height() >= option.decorationSize.height());
    assert(option.rect.size().width() >= option.decorationSize.width());

    const QRect& r = option.rect;
    QImage image = getImage(index, option.decorationSize);

    // There are three possible options now:
    // 1. image's rect is less than available area (option.decorationSize)
    //    - small image or some temporary 'loading in progress' icon
    //
    // 2. image is exactly of the same size as available area.
    //    This is most expected scenario - we have asked for option.decorationSize and we got it.
    //
    // 3. image is bigger than we want.
    //    It may happend if view's tails has different size/proporions that image.
    //    This should be temporary and view is supposed to fix it

    QRect imageRect = image.rect();
    int h_margin = (r.width()  - imageRect.width())  / 2;
    int v_margin = (r.height() - imageRect.height()) / 2;

    if (h_margin < 0 || v_margin < 0)  // case 3.
    {
        image = image.scaled(option.decorationSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        imageRect = image.rect();
        h_margin = (r.width()  - imageRect.width())  / 2;
        v_margin = (r.height() - imageRect.height()) / 2;
    }

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
