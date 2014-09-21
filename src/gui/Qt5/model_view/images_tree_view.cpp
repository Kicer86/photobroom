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

#include <cassert>
#include <unordered_map>


struct IndexHasher
{
    std::size_t operator()(const QModelIndex& index) const
    {
        return reinterpret_cast<std::size_t>(index.internalPointer());
    }
};


struct ImagesTreeView::Data
{
    std::unordered_map<QModelIndex, QRect, IndexHasher> m_positions;

    Data(): m_positions() {}
};


ImagesTreeView::ImagesTreeView(QWidget* _parent): QAbstractItemView(_parent), m_data(new Data)
{
    //setHeaderHidden(true);
}


ImagesTreeView::~ImagesTreeView()
{

}


QModelIndex ImagesTreeView::indexAt(const QPoint& point) const
{
    return QModelIndex();
}


bool ImagesTreeView::isIndexHidden(const QModelIndex& index) const
{
    return false;
}


QRect ImagesTreeView::visualRect(const QModelIndex& index) const
{
    return rectFor(index);
}


QRegion ImagesTreeView::visualRegionForSelection(const QItemSelection& selection) const
{
    return QRegion();
}


int ImagesTreeView::horizontalOffset() const
{
    return 0;
}


int ImagesTreeView::verticalOffset() const
{
    return 0;
}


QModelIndex ImagesTreeView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}


void ImagesTreeView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{

}


void ImagesTreeView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{

}


QRect ImagesTreeView::rectFor(const QModelIndex& index) const
{
    auto it = m_data->m_positions.find(index);

    if (it == m_data->m_positions.end())
    {
    }

    return it->second;
}


QPoint ImagesTreeView::positionOfNext(const QModelIndex& index) const
{
    assert(index.isValid());
    QRect rect = rectFor(index);


}


QPoint ImagesTreeView::matrixPositionOf(const QModelIndex& index) const
{
    assert(index.column() == 0);    // ImagesTreeView supports only typical hierarchical models. So column of item will be always equal to 0

    int linear_pos = index.row();

}

