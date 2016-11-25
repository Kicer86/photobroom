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

#include "data.hpp"


PositionsCalculator::PositionsCalculator(Data* data, int width): m_data(data), m_width(width)
{

}


PositionsCalculator::~PositionsCalculator()
{

}


void PositionsCalculator::updateItems() const
{
    assert(m_data->getModel().validate());

    updateItem(m_data->getModel().begin());
}


QRect PositionsCalculator::calcItemRect(Data::ModelIndexInfoSet::Model::const_level_iterator it) const
{
    QRect result;

    const Data::ModelIndexInfoSet::Model::const_level_iterator it_parent = it.parent();

    if (it_parent.valid())              //do not enter for top item
    {
        const QSize item_size = getItemSize(it);

        if (it.index() == 0)  //first
        {
            const QPoint point = calcPositionOfFirst(it);

            result = QRect(point, item_size);
        }
        else
        {
            Data::ModelIndexInfoSet::Model::const_level_iterator it_sibling = it - 1;
            const QPoint point = calcPositionOfNext(it_sibling);

            result = QRect(point, item_size);
        }
    }

    return result;
}

QSize PositionsCalculator::calcItemSize(Data::ModelIndexInfoSet::Model::const_iterator it) const
{
    const QSize result = isRoot(it)? QSize(0, 0): getItemSize(it);

    return result;
}


QPoint PositionsCalculator::calcItemPosition(Data::ModelIndexInfoSet::Model::const_level_iterator it) const
{
    QPoint result;

    Data::ModelIndexInfoSet::Model::const_level_iterator it_parent = it.parent();

    if (it_parent.valid())              //do not enter for top item
    {
        if (it.index() == 0)  //first
            result = calcPositionOfFirst(it);
        else
        {
            Data::ModelIndexInfoSet::Model::const_level_iterator it_sibling = it - 1;
            result = calcPositionOfNext(it_sibling);
        }
    }

    return result;
}


QPoint PositionsCalculator::calcPositionOfNext(Data::ModelIndexInfoSet::Model::const_iterator it) const
{
    const bool image = m_data->isImage(it);
    const QPoint result = image? calcPositionOfNextImage(it):
                                 calcPositionOfNextNode(it);

    return result;
}


QPoint PositionsCalculator::calcPositionOfNextImage(Data::ModelIndexInfoSet::Model::const_level_iterator infoIt) const
{
    const ModelIndexInfo& info = *infoIt;
    const QRect& item_pos = info.getRect();
    const Data::ModelIndexInfoSet::Model::const_level_iterator next_it = infoIt + 1;
    const int nextIndexWidth = getItemWidth(next_it);
    const Data::ModelIndexInfoSet::Model::const_level_iterator parentIt = infoIt.parent();
    const int max_width = m_width - m_data->getImageMargin() * 2;

    QPoint result;
    if (item_pos.right() + nextIndexWidth < max_width)             //is there place for item?
        result = QPoint(item_pos.x() + getItemWidth(infoIt), item_pos.y());
    else                                                         //no space, add new row
    {
        int row_height = 0;
        const std::pair<int, int> selection = selectRowFor(infoIt);
        Data::ModelIndexInfoSet::Model::const_level_iterator from = parentIt.begin() + selection.first;
        Data::ModelIndexInfoSet::Model::const_level_iterator to = parentIt.begin() + selection.second;

        for(Data::ModelIndexInfoSet::Model::const_level_iterator idxInfoIt = from; idxInfoIt != (to + 1); ++idxInfoIt)
        {
            const ModelIndexInfo& idxInfo = *idxInfoIt;
            const QRect& idxRect = idxInfo.getRect();
            const int idxHeight = idxRect.height();

            if (row_height < idxHeight)
                row_height = idxHeight;
        }

        result = QPoint(m_data->getImageMargin(), item_pos.y() + row_height);
    }

    return result;
}


QPoint PositionsCalculator::calcPositionOfNextNode(Data::ModelIndexInfoSet::Model::const_iterator infoIt) const
{
    assert(isRoot(infoIt) == false);

    const ModelIndexInfo& info = *infoIt;
    assert(info.isPositionValid());

    const QPoint& item_pos = info.getPosition();
    const QSize& items_size = info.getOverallSize();
    assert(info.isOverallSizeValid());

    const QPoint result = QPoint(0, item_pos.y() + items_size.height());

    return result;
}


QPoint PositionsCalculator::calcPositionOfFirst(Data::ModelIndexInfoSet::Model::const_iterator infoIt) const
{
    const bool image = m_data->isImage(infoIt);
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


int PositionsCalculator::getItemWidth(typename Data::ModelIndexInfoSet::Model::const_iterator infoIt) const
{
    int w = 0;
    if (m_data->isImage(infoIt))   //image
    {
        const QSize thumbSize = m_data->getThumbnailSize(infoIt);
        w = thumbSize.width() + m_data->getSpacing() * 2;
    }
    else                           //node's title
        w = m_width;

    return w;
}


int PositionsCalculator::getItemHeigth(Data::ModelIndexInfoSet::Model::const_iterator infoIt) const
{
    int item_height = 0;
    if (m_data->isImage(infoIt))   //image
    {
        const QSize thumbSize = m_data->getThumbnailSize(infoIt);
        item_height = thumbSize.height() + m_data->getSpacing() * 2;
    }
    else                           //node's title
        item_height = 40;          //TODO: temporary

    return item_height;
}


QSize PositionsCalculator::getItemSize(Data::ModelIndexInfoSet::Model::const_iterator infoIt) const
{
    const QSize item_size(getItemWidth(infoIt), getItemHeigth(infoIt));

    return item_size;
}


std::pair<int, int> PositionsCalculator::selectRowFor(Data::ModelIndexInfoSet::Model::const_level_iterator lastIt) const
{
    std::pair<int, int> result;
    Data::ModelIndexInfoSet::Model::const_level_iterator searchIt = lastIt;
    Data::ModelIndexInfoSet::Model::const_level_iterator firstIt = lastIt;

    const ModelIndexInfo& indexInfo = *lastIt;
    const QPoint& index_pos = indexInfo.getPosition();

    while(searchIt.index() > 0)
    {
        //go to previous item
        --searchIt;

        const QPoint& s_pos = searchIt->getPosition();

        if (s_pos.y() == index_pos.y())   //items are at the same y-position? If no - we are no longer in the same row
            firstIt = searchIt;
        else
            break;
    }

    return std::make_pair(firstIt.index(), lastIt.index());
}


int PositionsCalculator::getFirstItemOffset() const
{
    return 0;
}


bool PositionsCalculator::isRoot(Data::ModelIndexInfoSet::Model::const_level_iterator it) const
{
    Data::ModelIndexInfoSet::Model::const_level_iterator it_parent = it.parent();

    return it_parent.valid() == false;
}


void PositionsCalculator::updateItem(Data::ModelIndexInfoSet::Model::level_iterator infoIt) const
{
    ModelIndexInfo& info = *infoIt;

    // calculations only for dirty ones
    if (info.isPositionValid() == false)
    {
        const QPoint pos = calcItemPosition(infoIt);
        info.setPosition(pos);
    }

    if (info.isSizeValid() == false)
    {
        const QSize size = calcItemSize(infoIt);
        info.setSize(size);                       // size must be set at this point, as children calculations may require it
    }

    if (info.isOverallSizeValid() == false)
    {
        const bool& expanded = info.expanded;

        // update children
        if (expanded)
            for(Data::ModelIndexInfoSet::Model::level_iterator c_it = infoIt.begin(); c_it != infoIt.end(); ++c_it)
                updateItem(c_it);

        // calculate overall size
        QSize rect = info.getSize();

        //calculate overall only if node is expanded and has any children
        if (infoIt.children_count() != 0 && expanded)
        {
            const QPoint offset(0, info.getSize().height());

            for(Data::ModelIndexInfoSet::Model::level_iterator c_infoIt = infoIt.begin(); c_infoIt.valid(); ++c_infoIt)
            {
                const ModelIndexInfo& c_info = *c_infoIt;

                const QPoint c_relative_position = c_info.getPosition() + offset;
                const QSize c_overall_size = c_info.getOverallSize();
                assert(c_overall_size.isValid());

                const QSize c_size(c_overall_size.width() + c_relative_position.x(),
                                   c_overall_size.height() + c_relative_position.y());

                rect = rect.expandedTo(c_size);
            }

            rect.setHeight( rect.height() + m_data->getImageMargin() );
        }

        info.setOverallSize(rect);
    }
}
