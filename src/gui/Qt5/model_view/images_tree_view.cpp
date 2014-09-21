/*
 * Tree View for Images.
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

#include "images_tree_view.hpp"

ImagesTreeView::ImagesTreeView(QWidget* _parent): QAbstractItemView(_parent)
{
    //setHeaderHidden(true);
}


ImagesTreeView::~ImagesTreeView()
{

}


QModelIndex ImagesTreeView::indexAt(const QPoint& point) const
{
}


bool ImagesTreeView::isIndexHidden(const QModelIndex& index) const
{
}


QRect ImagesTreeView::visualRect(const QModelIndex& index) const
{
}


QRegion ImagesTreeView::visualRegionForSelection(const QItemSelection& selection) const
{
}


int ImagesTreeView::horizontalOffset() const
{
}


int ImagesTreeView::verticalOffset() const
{
}


QModelIndex ImagesTreeView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
}


void ImagesTreeView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{
}


void ImagesTreeView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{
}
