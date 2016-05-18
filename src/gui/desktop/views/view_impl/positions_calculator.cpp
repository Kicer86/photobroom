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

#include "data.hpp"

PositionsCalculator::PositionsCalculator(Data* data, int width): m_data(data), m_width(width)
{

}


PositionsCalculator::~PositionsCalculator()
{

}


void PositionsCalculator::updateItems() const
{
    updateItem(QModelIndex());
}


QSize PositionsCalculator::calcItemSize(const QModelIndex& idx) const
{
    const QSize result = idx.isValid()? getItemSize(idx): QSize(0, 0);

    return result;
}


QPoint PositionsCalculator::calcItemPosition(const QModelIndex& idx) const
{
    QPoint result;

    const QModelIndex parent = idx.parent();

    if (parent.isValid())              //do not enter for top item
    {
        if (idx.row() == 0)  //first
            result = calcPositionOfFirst(idx);
        else
        {
            const QModelIndex sibling = parent.child(idx.row() - 1, 0);
            result = calcPositionOfNext(sibling);
        }
    }

    return result;
}


QPoint PositionsCalculator::calcPositionOfNext(const QModelIndex& idx) const
{
    const bool image = m_data->isImage(idx);
    const QPoint result = image? calcPositionOfNextImage(idx):
                                 calcPositionOfNextNode(idx);

    return result;
}


QPoint PositionsCalculator::calcPositionOfNextImage(const QModelIndex& idx) const
{
    auto infoIt = m_data->get(idx);

    const ModelIndexInfo& info = infoIt->second;
    const QRect& item_pos = info.getRect();

    const QModelIndex next = idx.sibling(idx.row() + 1, 0);
    const int nextIndexWidth = getItemWidth(next);

    const QModelIndex parent = idx.parent();
    const int max_width = m_width - m_data->getImageMargin() * 2;

    QPoint result;
    if (item_pos.right() + nextIndexWidth < max_width)             //is there place for item?
        result = QPoint(item_pos.x() + getItemWidth(idx), item_pos.y());
    else                                                         //no space, add new row
    {
        int row_height = 0;
        const std::pair<int, int> selection = selectRowFor(idx);
        int from = selection.first;
        int to = selection.second;

        for(int i = from; i <= to; i++)
        {
            const QModelIndex item = parent.child(i, 0);
            auto it = m_data->get(item);

            const ModelIndexInfo& idxInfo = it->second;
            const QRect& idxRect = idxInfo.getRect();
            const int idxHeight = idxRect.height();

            if (row_height < idxHeight)
                row_height = idxHeight;
        }

        result = QPoint(m_data->getImageMargin(), item_pos.y() + row_height);
    }

    return result;
}


QPoint PositionsCalculator::calcPositionOfNextNode(const QModelIndex& idx) const
{
    assert(idx.isValid());

    auto infoIt = m_data->get(idx);

    const ModelIndexInfo& info = infoIt->second;
    assert(info.isPositionValid());

    const QPoint& item_pos = info.getPosition();
    const QSize& items_size = info.getOverallSize();
    assert(info.isOverallSizeValid());

    const QPoint result = QPoint(0, item_pos.y() + items_size.height());

    return result;
}


QPoint PositionsCalculator::calcPositionOfFirst(const QModelIndex& idx) const
{
    const bool image = m_data->isImage(idx);
    const QPoint result = image? calcPositionOfFirstImage():
                                 calcPositionOfFirstNode();

    return result;
}


QPoint PositionsCalculator::calcPositionOfFirstNode() const
{
    const int y_offset = getFirstItemOffset();
    const QPoint result(0, y_offset);

    return result;
}


QPoint PositionsCalculator::calcPositionOfFirstImage() const
{
    const int y_offset = getFirstItemOffset();
    const QPoint result(m_data->getImageMargin(), y_offset);

    return result;
}


int PositionsCalculator::getItemWidth(const QModelIndex& idx) const
{
    int w = 0;
    if (m_data->isImage(idx))   //image
    {
        const QSize thumbSize = m_data->getThumbnailSize(idx);
        w = thumbSize.width() + m_data->getSpacing() * 2;
    }
    else                           //node's title
        w = m_width;

    return w;
}


int PositionsCalculator::getItemHeigth(const QModelIndex& idx) const
{
    int item_height = 0;
    if (m_data->isImage(idx))   //image
    {
        const QSize thumbSize = m_data->getThumbnailSize(idx);
        item_height = thumbSize.height() + m_data->getSpacing() * 2;
    }
    else                           //node's title
        item_height = 40;          //TODO: temporary

    return item_height;
}


QSize PositionsCalculator::getItemSize(const QModelIndex& idx) const
{
    const QSize item_size(getItemWidth(idx), getItemHeigth(idx));

    return item_size;
}


std::pair<int, int> PositionsCalculator::selectRowFor(const QModelIndex& last) const
{
    std::pair<int, int> result(0, 0);

    QModelIndex first = last;
    QModelIndex search = first;

    auto it = m_data->get(first);
    const ModelIndexInfo& indexInfo = it->second;
    const QPoint& index_pos = indexInfo.getPosition();

    while(search.row() > 0)
    {
        //go to previous item
        search = search.sibling(search.row() - 1, 0);

        it = m_data->get(search);
        const QPoint& s_pos = it->second.getPosition();

        if (s_pos.y() == index_pos.y())   //items are at the same y-position? If no - we are no longer in the same row
            first = search;
        else
            break;
    }

    return std::make_pair(first.row(), last.row());
}


int PositionsCalculator::getFirstItemOffset() const
{
    return 0;
}


bool PositionsCalculator::isRoot(const QModelIndex& idx) const
{
    return idx.isValid() == false;
}


void PositionsCalculator::updateItem(const QModelIndex& idx) const
{
    auto it = m_data->get(idx);
    ModelIndexInfo& info = it->second;

    // calculations only for dirty ones
    if (info.isPositionValid() == false)
    {
        const QPoint pos = calcItemPosition(idx);
        info.setPosition(pos);
    }

    if (info.isSizeValid() == false)
    {
        const QSize size = calcItemSize(idx);
        info.setSize(size);                       // size must be set at this point, as children calculations may require it
    }

    if (info.isOverallSizeValid() == false)
    {
        const bool& expanded = info.expanded;
        int children = 0;

        // update children
        if (expanded)
            for_each_child(m_data->getItemModel(), idx, [&](const QModelIndex& child)
            {
                children++;
                updateItem(child);
            });

        // calculate overall size
        QSize rect = info.getSize();

        //calculate overall only if node is expanded and has any children
        if (children > 0)
        {
            const QPoint offset(0, info.getSize().height());

            for_each_child(m_data->getItemModel(), idx, [&](const QModelIndex& child)
            {
                auto it = m_data->get(child);
                const ModelIndexInfo& c_info = it->second;

                const QPoint c_relative_position = c_info.getPosition() + offset;
                const QSize c_overall_size = c_info.getOverallSize();
                assert(c_overall_size.isValid());

                const QSize c_size(c_overall_size.width() + c_relative_position.x(),
                                c_overall_size.height() + c_relative_position.y());

                rect = rect.expandedTo(c_size);
            });

            rect.setHeight( rect.height() + m_data->getImageMargin() );
        }

        info.setOverallSize(rect);
    }
}
