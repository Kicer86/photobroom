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
    assert(m_data->getModel().validate());

    updateItems(m_data->getModel().begin());
}


QRect PositionsCalculator::calcItemRect(Data::ModelIndexInfoSet::flat_iterator it) const
{
    QRect result;

    Data::ModelIndexInfoSet::flat_iterator it_parent = it.parent();

    if (it_parent.valid())              //do not enter for top item
    {
        const QSize item_size = getItemSize(it);

        if (it.index() == 0)  //first
        {
            const QPoint point = positionOfFirstChild(it_parent);

            result = QRect(point, item_size);
        }
        else
        {
            Data::ModelIndexInfoSet::flat_iterator it_sibling = it - 1;
            const QPoint point = positionOfNext(it_sibling);

            result = QRect(point, item_size);
        }
    }

    return result;
}

QSize PositionsCalculator::calcItemSize(Data::ModelIndexInfoSet::flat_iterator it) const
{
    const QSize result = isRoot(it)? QSize(): getItemSize(it);

    return result;
}


QPoint PositionsCalculator::calcItemPosition(Data::ModelIndexInfoSet::flat_iterator it) const
{
    QPoint result;

    Data::ModelIndexInfoSet::flat_iterator it_parent = it.parent();

    if (it_parent.valid())              //do not enter for top item
    {
        if (it.index() == 0)  //first
            result = positionOfFirstChild(it_parent);
        else
        {
            Data::ModelIndexInfoSet::flat_iterator it_sibling = it - 1;
            result = positionOfNext(it_sibling);
        }
    }

    return result;
}


QPoint PositionsCalculator::positionOfNext(Data::ModelIndexInfoSet::flat_iterator it) const
{
    const bool image = m_data->isImage(it);
    const QPoint result = image? positionOfNextImage(it):
                                 positionOfNextNode(it);

    return result;
}


QPoint PositionsCalculator::positionOfNextImage(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    const ModelIndexInfo& info = *infoIt;
    const QRect& item_pos = info.getRect();
    Data::ModelIndexInfoSet::flat_iterator next_it = infoIt + 1;
    const int nextIndexWidth = getitemWidth(next_it);
    Data::ModelIndexInfoSet::flat_iterator parentIt = infoIt.parent();

    QPoint result;
    if (item_pos.right() + nextIndexWidth < m_width)             //is there place for item?
        result = QPoint(item_pos.x() + getitemWidth(infoIt), item_pos.y());
    else                                                         //no space, add new row
    {
        int row_height = 0;
        const std::pair<int, int> selection = selectRowFor(infoIt);
        Data::ModelIndexInfoSet::flat_iterator from = parentIt.begin() + selection.first;
        Data::ModelIndexInfoSet::flat_iterator to = parentIt.begin() + selection.second;

        for(Data::ModelIndexInfoSet::flat_iterator idxInfoIt = from; idxInfoIt != to; ++idxInfoIt)
        {
            const ModelIndexInfo& idxInfo = *idxInfoIt;
            const QRect& idxRect = idxInfo.getRect();
            const int idxHeight = idxRect.height();

            if (row_height < idxHeight)
                row_height = idxHeight;
        }

        result = QPoint(0, item_pos.y() + row_height);
    }

    return result;
}


QPoint PositionsCalculator::positionOfNextNode(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    assert(isRoot(infoIt) == false);

    const ModelIndexInfo& info = *infoIt;
    assert(info.isPositionValid());

    const QPoint& item_pos = info.getPosition();
    const QSize& items_size = info.getOverallSize();
    assert(items_size.isValid());

    const QPoint result = QPoint(0, item_pos.y() + items_size.height());

    return result;
}


QPoint PositionsCalculator::positionOfFirstChild(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    QPoint result(0, 0);

    if (isRoot(infoIt) == false)           // regular item
    {
        const QRect r = calcItemRect(infoIt);
        result = QPoint(0, r.y() + r.height());
    }

    return result;
}


int PositionsCalculator::getitemWidth(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    int w = 0;
    if (m_data->isImage(infoIt))   //image
    {
        QPixmap pixmap = m_data->getImage(infoIt);
        w = pixmap.width() + m_data->indexMargin * 2;
    }
    else                           //node's title
        w = m_width;

    return w;
}


int PositionsCalculator::getItemHeigth(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    int item_height = 0;
    if (m_data->isImage(infoIt))   //image
    {
        QPixmap pixmap = m_data->getImage(infoIt);
        item_height = pixmap.height() + m_data->indexMargin * 2;
    }
    else                           //node's title
        item_height = 40;          //TODO: temporary

    return item_height;
}


QSize PositionsCalculator::getItemSize(Data::ModelIndexInfoSet::flat_iterator infoIt) const
{
    const QSize item_size(getitemWidth(infoIt), getItemHeigth(infoIt));

    return item_size;
}


std::pair<int, int> PositionsCalculator::selectRowFor(Data::ModelIndexInfoSet::flat_iterator lastIt) const
{
    std::pair<int, int> result;
    Data::ModelIndexInfoSet::flat_iterator searchIt = lastIt;
    Data::ModelIndexInfoSet::flat_iterator firstIt = lastIt;

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


bool PositionsCalculator::isRoot(ViewDataSet<ModelIndexInfo>::flat_iterator it) const
{
    Data::ModelIndexInfoSet::flat_iterator it_parent = it.parent();

    return it_parent.valid() == false;
}


void PositionsCalculator::updateItems(Data::ModelIndexInfoSet::flat_iterator item) const
{
    const bool invalid = item->valid() == false;

    if (invalid)
    {
        const bool expanded = m_data->isExpanded(item);

        if (expanded)
            for(Data::ModelIndexInfoSet::flat_iterator c_it = item.begin(); c_it.valid(); ++c_it)
                updateItems(c_it);

        updateItem(item);
    }
}


void PositionsCalculator::updateItem(Data::ModelIndexInfoSet::flat_iterator infoIt) const
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
        info.setSize(size);                       // size muse be set at this point, as children calculations may require it
    }

    if (info.getOverallSize().isValid() == false)
    {
        QSize rect = info.getSize();

        //calculate overall only if node is expanded and has any children
        if (infoIt.children_count() != 0 && m_data->isExpanded(infoIt))
        {
            const QPoint offset = info.getPosition();

            for(Data::ModelIndexInfoSet::flat_iterator c_infoIt = infoIt.begin(); c_infoIt.valid(); ++c_infoIt)
            {
                const ModelIndexInfo& c_info = *c_infoIt;

                const QPoint c_relative_position = c_info.getPosition() - offset;
                const QSize c_overall_size = c_info.getOverallSize();
                assert(c_overall_size.isValid());

                const QSize c_size(c_overall_size.width() + c_relative_position.x(),
                                   c_overall_size.height() + c_relative_position.y());
                
                rect = rect.expandedTo(c_size);
            }
        }

        info.setOverallSize(rect);
    }
}
