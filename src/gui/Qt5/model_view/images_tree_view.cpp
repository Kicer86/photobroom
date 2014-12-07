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

#include <QScrollBar>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

#include <configuration/constants.hpp>
#include <configuration/configuration.hpp>

#include "view_helpers/data.hpp"
#include "view_helpers/positions_calculator.hpp"
#include "view_helpers/positions_reseter.hpp"
#include "tree_item_delegate.hpp"


ImagesTreeView::ImagesTreeView(QWidget* _parent): QAbstractItemView(_parent), m_data(new Data)
{
    TreeItemDelegate* delegate = new TreeItemDelegate(this);

    setItemDelegate(delegate);
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
    const QPoint offset = getOffset();
    const QPoint treePoint = point + offset;
    const ModelIndexInfo info = m_data->get(treePoint);
    const QModelIndex& result = info.index;

    return result;
}


bool ImagesTreeView::isIndexHidden(const QModelIndex& index) const
{
    (void) index;

    return false;
}


QRect ImagesTreeView::visualRect(const QModelIndex& index) const
{
    return getItemRect(index);
}


QRegion ImagesTreeView::visualRegionForSelection(const QItemSelection& selection) const
{
    QModelIndexList indexes = selection.indexes();
    QRegion result;

    for (const QModelIndex& idx: indexes)
    {
        ModelIndexInfo info = m_data->get(idx);

        result += info.getRect();
    }

    return result;
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
    (void) cursorAction;
    (void) modifiers;

    return QModelIndex();
}


void ImagesTreeView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{
    (void) index;
    (void) hint;
}


void ImagesTreeView::setSelection(const QRect& _rect, QItemSelectionModel::SelectionFlags flags)
{
    const QRect treeRect = _rect.translated(getOffset());

    const std::deque<QModelIndex> items = findItemsIn(treeRect);
    QItemSelection selection;

    for (const QModelIndex& item: items)
        selection.select(item, item);

    selectionModel()->select(selection, flags);
}


void ImagesTreeView::setModel(QAbstractItemModel* m)
{
    //disconnect current model
    QAbstractItemModel* current_model = QAbstractItemView::model();
    disconnect(current_model);

    //
    QAbstractItemView::setModel(m);

    //read model
    rereadModel();

    //connect to model's signals
    connect(m, SIGNAL(modelReset()), this, SLOT(modelReset()));
    connect(m, SIGNAL(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)),
            this, SLOT(rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int)));
    connect(m, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SLOT(rowsMoved(QModelIndex,int,int,QModelIndex,int)));
    connect(m, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
}


void ImagesTreeView::rowsInserted(const QModelIndex& _parent, int from, int to)
{
    QAbstractItemView::rowsInserted(_parent, from, to);

    PositionsReseter reseter(m_data.get());
    reseter.itemsAdded(_parent, to);

    updateModel();
}


void ImagesTreeView::rowsAboutToBeRemoved(const QModelIndex& _parent, int start, int end)
{
    QAbstractItemView::rowsAboutToBeRemoved(_parent, start, end);

    //remove data from internal data model
    for(int i = start; i <= end; i++)
    {
        QModelIndex child = model()->index(i, 0, _parent);

        m_data->forget(child);
    }
}


void ImagesTreeView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());
    const QPoint offset = getOffset();
    QRect visible_area = viewport()->rect();

    visible_area.moveTo(offset);
    painter.translate(-offset);

    std::deque<QModelIndex> items = findItemsIn(visible_area);

    for (const QModelIndex& item: items)
    {
        ModelIndexInfo info = m_data->get(item);

        QStyleOptionViewItem styleOption;
        styleOption.rect = info.getRect();
        styleOption.features = m_data->isImage(item)? QStyleOptionViewItem::HasDecoration: QStyleOptionViewItem::HasDisplay;
        styleOption.palette = palette();
        styleOption.state |= selectionModel()->isSelected(item)? QStyle::State_Selected: QStyle::State_None;

        QAbstractItemView::itemDelegate()->paint(&painter, styleOption, item);
    }
}


void ImagesTreeView::mouseReleaseEvent(QMouseEvent* e)
{
    QAbstractItemView::mouseReleaseEvent(e);

    QModelIndex item = indexAt(e->pos());
    ModelIndexInfo info = m_data->get(item);

    if (item.isValid())
    {
        info.expanded = !info.expanded;
        m_data->update(info);

        //reset some positions
        PositionsReseter reseter(m_data.get());
        reseter.itemChanged(item);

        updateModel();
    }
}


void ImagesTreeView::resizeEvent(QResizeEvent* e)
{
    QAbstractItemView::resizeEvent(e);

    //reset all positions
    PositionsReseter reseter(m_data.get());
    reseter.invalidateAll();

    updateModel();
}


/// private:


const QRect& ImagesTreeView::getItemRect(const QModelIndex& index) const
{
    const ModelIndexInfo& info = m_data->get(index);

    return info.getRect();
}


std::deque<QModelIndex> ImagesTreeView::findItemsIn(const QRect& _rect) const
{
    //TODO: optimise?
    std::deque<QModelIndex> result;

    m_data->for_each_visible( [&] (const ModelIndexInfo& info)
    {
        const QRect& item_rect = info.getRect();
        const QModelIndex& index = info.index;
        const bool overlap = _rect.intersects(item_rect);

        if (overlap)
            result.push_back(index);

        return true;
    });

    return result;
}


std::deque<QModelIndex> ImagesTreeView::getChildrenFor(const QModelIndex &) const
{
    std::deque<QModelIndex> result;

    m_data->for_each_recursively(QAbstractItemView::model(), [&] (const QModelIndex &, const std::deque<QModelIndex>& _children)
    {
        result.insert(result.end(), _children.begin(), _children.end());
    });

    return result;
}


void ImagesTreeView::rereadModel()
{
    m_data->clear();
    updateModel();
}


void ImagesTreeView::updateModel()
{
    QAbstractItemModel* m = QAbstractItemView::model();

    // is there anything to calculate?
    if (m != nullptr && m->rowCount() > 0)
    {
        PositionsCalculator calculator(m, m_data.get(), viewport()->width());
        calculator.updateItems();
    }

    updateGui();
}


void ImagesTreeView::updateGui()
{
    const ModelIndexInfo info = m_data->get(QModelIndex());
    const QSize areaSize = viewport()->size();
    const QSize treeAreaSize = info.getOverallRect().size();

    verticalScrollBar()->setPageStep(areaSize.height());
    horizontalScrollBar()->setPageStep(areaSize.width());
    verticalScrollBar()->setRange(0, treeAreaSize.height() - areaSize.height());
    horizontalScrollBar()->setRange(0, treeAreaSize.width() - areaSize.width());

    //refresh widget
    viewport()->update();
}


QPoint ImagesTreeView::getOffset() const
{
    const QPoint offset(horizontalOffset(), verticalOffset());

    return offset;
}


void ImagesTreeView::modelReset()
{
    rereadModel();
}


void ImagesTreeView::updateModelShot()
{
    updateModel();
}


void ImagesTreeView::rowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow)
{
    const int items = sourceEnd - sourceStart + 1;
    rowsAboutToBeRemoved(sourceParent, sourceStart, sourceEnd);
}


void ImagesTreeView::rowsMoved(const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow)
{
    const int items = sourceEnd - sourceStart + 1;
    rowsRemoved(sourceParent, sourceStart, sourceEnd);
    rowsInserted(destinationParent, destinationRow, destinationRow + items - 1);
}


void ImagesTreeView::rowsRemoved(const QModelIndex& _parent, int first, int)
{
    //reset sizes and positions of existing items
    PositionsReseter reseter(m_data.get());
    reseter.childrenRemoved(_parent, first);

    //update model
    updateModel();
}
