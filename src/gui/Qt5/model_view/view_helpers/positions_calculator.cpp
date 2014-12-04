/*
 * QModelIndex position calculator
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

#include "positions_calculator.hpp"

#include <cassert>

#include <QModelIndex>

#include <configuration/constants.hpp>
#include <configuration/configuration.hpp>

#include "data.hpp"

PositionsCalculator::PositionsCalculator(QAbstractItemModel* model, Data* data, int width): m_model(model), m_data(data), m_width(width)
{

}


PositionsCalculator::~PositionsCalculator()
{

}


void PositionsCalculator::updateItems() const
{
    m_data->for_each_recursively(m_model, [&](const QModelIndex& idx, const std::deque<QModelIndex>& children)
    {
        ModelIndexInfo info = m_data->get(idx);

        // calculations only for dirty ones
        if (info.getRect().isNull())
        {
            QRect rect = calcItemRect(idx);
            info.setRect(rect);
            m_data->update(info);                                        // size muse be stored at this point, as children calculations may require it
        }

        if (info.getOverallRect().isNull())
        {
            QRect rect = info.getRect();
            for(const QModelIndex& child: children)
            {
                ModelIndexInfo c_info = m_data->get(child);
                QRect c_rect = c_info.getOverallRect();
                assert(c_rect.isValid());

                rect = rect.united(c_rect);
            }

            info.setOverallRect(rect);
            m_data->update(info);
        }
    });
}


QRect PositionsCalculator::calcItemRect(const QModelIndex& index) const
{
    QRect result;

    if (index != QModelIndex())
    {
        QModelIndex item_parent = m_model->parent(index);
        const QSize item_size = getItemSize(index);

        if (index.row() == 0)  //first
        {
            const QPoint point = positionOfFirstChild(item_parent);

            result = QRect(point, item_size);
        }
        else
        {
            const QModelIndex sibling = m_model->index(index.row() - 1, 0, item_parent);
            const QPoint point = positionOfNext(sibling);

            result = QRect(point, item_size);
        }
    }

    return result;
}


QPoint PositionsCalculator::positionOfNext(const QModelIndex& index) const
{
    const bool image = m_data->isImage(index);
    const QPoint result = image? positionOfNextImage(index):
                                 positionOfNextNode(index);

    return result;
}


QPoint PositionsCalculator::positionOfNextImage(const QModelIndex& index) const
{
    assert(index.isValid());

    const ModelIndexInfo& info = m_data->get(index);
    const QRect& item_pos = info.getRect();
    const QModelIndex nextIndex = index.sibling(index.row() + 1, 0);
    const int nextIndexWidth = getitemWidth(nextIndex);

    QPoint result;
    if (item_pos.right() + nextIndexWidth < m_width)             //is there place for item?
        result = QPoint(item_pos.x() + getitemWidth(index), item_pos.y());
    else                                                         //no space, add new row
    {
        int row_height = 0;
        const QItemSelection selection = selectRowFor(index);
        for(const QModelIndex& idx: selection.indexes())
        {
            ModelIndexInfo idxInfo = m_data->get(idx);
            const QRect& idxRect = idxInfo.getRect();
            const int idxHeight = idxRect.height();

            if (row_height < idxHeight)
                row_height = idxHeight;
        }

        result = QPoint(0, item_pos.y() + row_height);
    }

    return result;
}


QPoint PositionsCalculator::positionOfNextNode(const QModelIndex& index) const
{
    assert(index.isValid());

    ModelIndexInfo info = m_data->get(index);
    const QRect items_pos = info.getOverallRect();
    assert(items_pos.isValid());
    const QPoint result = QPoint(0, items_pos.bottom());

    return result;
}


QPoint PositionsCalculator::positionOfFirstChild(const QModelIndex& index) const
{
    QPoint result(0, 0);

    if (index != QModelIndex())           // regular item
    {
        const QRect r = calcItemRect(index);
        result = QPoint(0, r.y() + r.height());
    }

    return result;
}


int PositionsCalculator::getitemWidth(const QModelIndex& index) const
{
    int w = 0;
    if (m_data->isImage(index))   //image
    {
        QPixmap pixmap = m_data->getImage(index);
        w = pixmap.width() + m_data->indexMargin * 2;
    }
    else                  //node's title
        w = m_width;

    return w;
}


int PositionsCalculator::getItemHeigth(const QModelIndex& index) const
{
    int item_height = 0;
    if (m_data->isImage(index))   //image
    {
        QPixmap pixmap = m_data->getImage(index);
        item_height = pixmap.height() + m_data->indexMargin * 2;
    }
    else                  //node's title
        item_height = 40;      //TODO: temporary

    return item_height;
}


QSize PositionsCalculator::getItemSize(const QModelIndex& index) const
{
    const QSize item_size(getitemWidth(index), getItemHeigth(index));

    return item_size;
}


QItemSelection PositionsCalculator::selectRowFor(const QModelIndex& index) const
{
    QItemSelection result;
    QModelIndex itemToCheck = index;

    while(itemToCheck.isValid())
    {
        const QModelIndex current = itemToCheck;
        const ModelIndexInfo indexInfo = m_data->get(itemToCheck);
        const QRect& indexRect = indexInfo.getRect();

        //go to previous item
        itemToCheck = itemToCheck.sibling(itemToCheck.row() - 1, 0);

        if (itemToCheck.isValid())
        {
            const ModelIndexInfo prevInfo = m_data->get(itemToCheck);
            const QRect& prevRect = prevInfo.getRect();

            if (prevRect.top() != indexRect.top())   //items are at the same y-position? If no - we are no longer in the same row
                itemToCheck = QModelIndex();         //mark item invalid
        }

        if (itemToCheck.isValid() == false)
            result.select(current, index);
    }

    return result;
}

