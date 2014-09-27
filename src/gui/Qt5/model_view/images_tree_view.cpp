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

#include <configuration/constants.hpp>
#include <configuration/configuration.hpp>

#include "view_helpers/data.hpp"
#include "view_helpers/positions_calculator.hpp"


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
    Data::ModelIndexInfo info = m_data->get(point);
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
    (void) selection;

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
    (void) cursorAction;
    (void) modifiers;

    return QModelIndex();
}


void ImagesTreeView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{
    (void) index;
    (void) hint;
}


void ImagesTreeView::setSelection(const QRect& _rect, QItemSelectionModel::SelectionFlags command)
{
    (void) _rect;
    (void) command;
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
    connect(m, SIGNAL(modelReset()),                        this, SLOT(modelReset()));
    connect(m, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(rowsInserted(QModelIndex, int, int)));
}


void ImagesTreeView::paintEvent(QPaintEvent *)
{
    QPainter painter(viewport());

    QRect visible_area = QWidget::rect();
    visible_area.moveTo(horizontalOffset(), verticalOffset());

    std::deque<QModelIndex> items = findItemsIn(visible_area);

    for(const QModelIndex& item: items)
    {
        /*
        QStyleOptionViewItem styleOption;
        QAbstractItemView::itemDelegate()->paint(&painter, styleOption, item);
        */

        QAbstractItemModel* m = QAbstractItemView::model();
        const Data::ModelIndexInfo& info = m_data->get(item);
        const QRect& r = info.rect;
        const bool image = m_data->isImage(m, item);

        if (image)
        {
            const QVariant v = m->data(item, Qt::DecorationRole);
            const QPixmap p = v.value<QPixmap>();

            painter.drawPixmap(m_data->indexMargin, m_data->indexMargin, p);
        }
        else
        {
            const QVariant v = m->data(item, Qt::DisplayRole);
            const QString t = v.toString();

            painter.drawText(r, Qt::AlignCenter, t);
        }
    }

}


void ImagesTreeView::mouseReleaseEvent(QMouseEvent* e)
{
    QAbstractScrollArea::mouseReleaseEvent(e);

    Data::ModelIndexInfo info = m_data->get(e->pos());
    const QModelIndex& item = info.index;

    if (item.isValid())
    {
        const bool expanded = isExpanded(item);

        if (expanded)
        {
        }
    }
}


/// private:


const QRect& ImagesTreeView::getItemRect(const QModelIndex& index) const
{
    const Data::ModelIndexInfo& info = m_data->get(index);

    return info.rect;
}


std::deque<QModelIndex> ImagesTreeView::findItemsIn(const QRect& _rect) const
{
    //TODO: optimise?
    std::deque<QModelIndex> result;

    m_data->for_each( [&] (const Data::ModelIndexInfo& info)
    {
        const QRect& item_rect = info.rect;
        const QModelIndex& index = info.index;
        const bool overlap = _rect.intersects(item_rect);

        if (overlap)
            result.push_back(index);

        return true;
    });

    return result;
}


std::deque<QModelIndex> ImagesTreeView::getChildrenFor(const QModelIndex& _parent) const
{
    std::deque<QModelIndex> result;

    QAbstractItemModel* m = QAbstractItemView::model();
    const bool has_children = m->hasChildren();

    if (has_children)
    {
        const int r = m->rowCount(_parent);
        for(int i = 0; i < r; i++)
        {
            QModelIndex c = m->index(i, 0, _parent);
            result.push_back(c);

            const bool expanded = isExpanded(c);
            if (expanded)
            {
                std::deque<QModelIndex> c_result = getChildrenFor(c);
                result.insert(result.end(), c_result.begin(), c_result.end());
            }
        }
    }

    return result;
}


bool ImagesTreeView::isExpanded(const QModelIndex& index) const
{
    const Data::ModelIndexInfo& info = m_data->get(index);

    return info.expanded;
}


void ImagesTreeView::rereadModel()
{
    m_data->clear();
    std::deque<QModelIndex> items = getChildrenFor(QModelIndex());

    for(const QModelIndex& index: items)
    {
        QAbstractItemModel* m = QAbstractItemView::model();
        PositionsCalculator calculator(m, m_data.get(), QWidget::width());
        QRect itemRect = calculator.calcItemRect(index);

        //check if we are ok with order
        /*
        if (m_data->m_visibleItemsMap.empty() == false)
        {
            const QRectCompare comparer;
            const auto& item = m_data->m_visibleItemsMap.back();
            const bool comp = comparer(item.first, itemRect);    //current item should be greater than last one

            assert(comp);
        }
        */
        Data::ModelIndexInfo info(index);
        info.rect = itemRect;

        m_data->add(info);
    }

    //refresh widget
    update();
}


void ImagesTreeView::modelReset()
{
    rereadModel();
}


void ImagesTreeView::rowsInserted(const QModelIndex&, int, int)
{
    //TODO: optimise
    rereadModel();
}
