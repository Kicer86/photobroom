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

#include <QScrollBar>
#include <QPainter>

#include <configuration/constants.hpp>
#include <configuration/configuration.hpp>

namespace
{

    struct IndexHasher
    {
        std::size_t operator()(const QModelIndex& index) const
        {
            return reinterpret_cast<std::size_t>(index.internalPointer());
        }
    };

    const int indexMargin = 10;           // TODO: move to configuration
}


struct ImagesTreeView::Data
{
    struct ModelIndexInfo
    {
        QRect rect;
        bool expanded;

        ModelIndexInfo(): rect(), expanded(false) {}
    };

    std::unordered_map<QModelIndex, QRect, IndexHasher> m_positions;
    IConfiguration* m_configuration;

    Data(): m_positions(), m_configuration(nullptr) {}
};


ImagesTreeView::ImagesTreeView(QWidget* _parent): QAbstractItemView(_parent), m_data(new Data)
{
    //setHeaderHidden(true);
}


ImagesTreeView::~ImagesTreeView()
{

}


void ImagesTreeView::set(IConfiguration* configuration)
{
    m_data->m_configuration = configuration;
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
    return getItemRect(index);
}


QRegion ImagesTreeView::visualRegionForSelection(const QItemSelection& selection) const
{
    return QRegion();
}


int ImagesTreeView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}


int ImagesTreeView::verticalOffset() const
{
    return verticalScrollBar()->value();
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


void ImagesTreeView::paintEvent(QPaintEvent* event)
{
    QPainter painter(viewport());

    QRect visible_area= QWidget::rect();
    visible_area.moveTo(horizontalOffset(), verticalOffset());



    /*
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
    */
}


/// private:

QRect ImagesTreeView::getItemRect(const QModelIndex& index) const
{
    auto it = m_data->m_positions.find(index);

    if (it == m_data->m_positions.end())
    {
        assert(index.column() == 0);

        QModelIndex parent = QAbstractItemView::model()->parent(index);
        QRect rect;

        if (index.row() == 0)  //first
        {
            const QPoint point = positionOfFirstChild(parent);
            const QSize size = getItemSize(index);

            rect = QRect(point, size);
        }
        else
        {
            const QModelIndex sibling = QAbstractItemView::model()->index(index.row() - 1, 0, parent);
            const QPoint point = positionOfNext(sibling);
            const QSize size = getItemSize(index);

            rect = QRect(point, size);
        }

        auto data = std::make_pair(index, rect);
        auto insert_it = m_data->m_positions.insert(data);

        it = insert_it.first;
    }

    return it->second;
}


QPoint ImagesTreeView::positionOfNext(const QModelIndex& index) const
{
    assert(index.isValid());
    QRect rect = getItemRect(index);

    const int items_per_row = itemsPerRow();
    const QPoint items_matrix_pos = matrixPositionOf(index);
    const QRect items_pos = getItemRect(index);

    assert(items_matrix_pos.x() < items_per_row);

    QPoint result;
    if (items_pos.x() + 1 < items_per_row)             //not last in its row?
        result = QPoint(items_pos.x() + getitemWidth(index), items_pos.y());
    else                                               //last in a row
    {
        QModelIndex parent = QAbstractItemView::model()->parent(index);
        QModelIndex from = itemAtMatrixPosition(QPoint(0, items_matrix_pos.y()), parent);
        QModelIndex to = itemAtMatrixPosition(QPoint(items_per_row - 1, items_matrix_pos.y()), parent);

        const int height = getItemHeigth(from, to);

        result = result = QPoint(0, items_pos.y() + height);
    }

    return result;
}


QPoint ImagesTreeView::positionOfFirstChild(const QModelIndex& index) const
{
    QPoint result;

    if (index.isValid())           // regular item
    {
        const QRect rect = getItemRect(index);
        result = QPoint(0, rect.y() + rect.height());
    }
    else                           // master root
        result = QPoint(0, 0);

    return result;
}


QPoint ImagesTreeView::matrixPositionOf(const QModelIndex& index) const
{
    assert(index.column() == 0);    // ImagesTreeView supports only typical hierarchical models. So column of item will be always equal to 0

    const int linear_pos = index.row();
    const int indicesPerRow = itemsPerRow();
    const int row = linear_pos / indicesPerRow;
    const int col = linear_pos % indicesPerRow;

    return QPoint(col, row);
}


QModelIndex ImagesTreeView::itemAtMatrixPosition(const QPoint& point, QModelIndex& parent) const
{
    const int indicesPerRow = itemsPerRow();
    const int liner_pos = indicesPerRow * point.y() + point.x();

    QModelIndex item = QAbstractItemView::model()->index(liner_pos, 0, parent);

    return item;
}


int ImagesTreeView::itemsPerRow() const
{
    const int indexWidth = m_data->m_configuration->findEntry(Configuration::BasicKeys::thumbnailWidth, "120").toInt();
    const int widgetWidth = QWidget::width();
    const int indicesPerRowInitial = widgetWidth / indexWidth;
    const int indicesPerRow = indicesPerRowInitial > 1? indicesPerRowInitial : 2;    // at least 2 items per row

    return indicesPerRow;
}


int ImagesTreeView::getitemWidth(const QModelIndex& index) const
{
    int width = 0;
    if (isImage(index))   //image
    {
        QPixmap pixmap = getImage(index);
        width = pixmap.width() + indexMargin;
    }
    else                  //node's title
        width = QWidget::width();

    return width;
}


bool ImagesTreeView::isImage(const QModelIndex& index) const
{
    QAbstractItemModel* model = QAbstractItemView::model();
    const bool children = model->hasChildren(index);
    bool result = false;

    if (!children)     //has no children? Leaf (image) or empty node, so still not sure
    {
        QPixmap pixmap = getImage(index);

        result = pixmap.isNull() == false;
    }
    //else - has children so it is node so it is not image :)

    return result;
}


QPixmap ImagesTreeView::getImage(const QModelIndex& index) const
{
    const QVariant decorationRole = QAbstractItemView::model()->data(index, Qt::DecorationRole);  //get display role
    const bool convertable = decorationRole.canConvert<QPixmap>();
    QPixmap pixmap;

    if (convertable)
        pixmap = decorationRole.value<QPixmap>();

    return pixmap;
}


int ImagesTreeView::getItemHeigth(const QModelIndex& index) const
{
    int height = 0;
    if (isImage(index))   //image
    {
        QPixmap pixmap = getImage(index);
        height = pixmap.height() + indexMargin;
    }
    else                  //node's title
        height = 40;      //TODO: temporary

    return height;
}


int ImagesTreeView::getItemHeigth(const QModelIndex& from, const QModelIndex& to) const
{
    int result = 0;
    QModelIndex parent = QAbstractItemView::model()->parent(from);
    assert(parent == QAbstractItemView::model()->parent(to));

    for (int i = from.row(); i < to.row(); i++)
    {
        QModelIndex index = QAbstractItemView::model()->index(i, 0, parent);

        const int h = getItemHeigth(index);
        if (result < h)
            result = h;
    }

    return result;
}


QSize ImagesTreeView::getItemSize(const QModelIndex& index) const
{
    const QSize size(getitemWidth(index), getItemHeigth(index));

    return size;
}


std::deque<QModelIndex> ImagesTreeView::findItemsIn(const QRect& rect) const
{
    //TODO: optimise?
    const std::deque<QModelIndex> result = getChildrenFor(QModelIndex());


}


std::deque<QModelIndex> ImagesTreeView::getChildrenFor(const QModelIndex& parent) const
{
    std::deque<QModelIndex> result;

    QAbstractItemModel* m = QAbstractItemView::model();
    const bool children = m->hasChildren();

    if (children)
    {
        const int r = m->rowCount(parent);
        for(int i = 0; i < r; i++)
        {
            QModelIndex c = m->index(r, 0, parent);
            result.push_back(c);

            std::deque<QModelIndex> c_result = getChildrenFor(c);
            result.insert(result.end(), c_result.begin(), c_result.end());
        }
    }

    return result;
}
