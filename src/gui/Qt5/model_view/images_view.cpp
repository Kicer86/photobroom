/*
    Images view for Model.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "images_view.hpp"

#include <QPainter>
#include <QDebug>
#include <QScrollBar>

#include "positions_cache.hpp"
#include "image_manager.hpp"

ImagesView::ImagesView(QWidget* p) : QAbstractItemView(p), m_cache( new PositionsCache(this) ) {}


ImagesView::~ImagesView()
{

}


QModelIndexList ImagesView::getSelection() const
{
    return selectedIndexes();
}


void ImagesView::paintEvent(QPaintEvent*)
{
    //TODO: use itemDelegate() for painting
    QPainter painter(viewport());

    const int items = m_cache->items();
    QAbstractItemModel* dataModel = model();
    ImageManager imageManager(dataModel);

    for (int i = 0; i < items; i++)
    {
        QModelIndex index = model()->index(i, 0);
        const QRect position = m_cache->pos(i);

        //paint selection
        const bool selected = selectionModel()->isSelected(index);

        if (selected)
        {
            painter.setPen(QColor(0, 0, 0, 0));
            painter.setBrush(QBrush(QColor(0, 0, 255)));
            painter.drawRect(position);
        }

        //paint image
        imageManager.draw(i, &painter, position);
    }
}


void ImagesView::resizeEvent(QResizeEvent* e)
{
    m_cache->invalidate();
    QAbstractItemView::resizeEvent(e);
}


QRect ImagesView::visualRect(const QModelIndex& index) const
{
    QAbstractItemModel* dataModel = model();
    QRect result;

    if (dataModel != nullptr && index.isValid())
    {
        const int row = index.row();
        result = m_cache->pos(row);
    }

    return result;
}


void ImagesView::scrollTo(const QModelIndex& idx, QAbstractItemView::ScrollHint hint)
{
    (void) idx;
    (void) hint;
}


QModelIndex ImagesView::indexAt(const QPoint& point) const
{
    QModelIndex result;

    for(size_t i = 0; i < m_cache->items(); i++)
    {
        const QRect& position = m_cache->pos(i);

        if (position.contains(point))
        {
            result = model()->index(i, 0);
            break;
        }
    }

    qDebug() << result;

    return result;
}


QModelIndex ImagesView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    (void) cursorAction;
    (void) modifiers;
    QModelIndex result;

    return result;
}


int ImagesView::horizontalOffset() const
{
    return 0;
}


int ImagesView::verticalOffset() const
{
    return verticalScrollBar()->value();
}


bool ImagesView::isIndexHidden(const QModelIndex& index) const
{
    (void) index;
    return false;
}


void ImagesView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{
    QItemSelection selection;

    //find all items in rect
    for(size_t i = 0; i < m_cache->items(); i++)
    {
        QRect item = m_cache->pos(i);

        if ( (rect & item).isEmpty() == false )
        {
            QModelIndex index = model()->index(i, 0);

            selection.select(index, index);
        }
    }

    selectionModel()->select(selection, command);
}


QRegion ImagesView::visualRegionForSelection(const QItemSelection& selection) const
{
    (void) selection;
    QRegion result;

    return result;
}


void ImagesView::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector< int >& roles)
{
    m_cache->invalidate();
    QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
}


void ImagesView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    m_cache->invalidate();
    QAbstractItemView::rowsInserted(parent, start, end);
}


void ImagesView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    m_cache->invalidate();
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}
