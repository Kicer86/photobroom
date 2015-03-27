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

    m_data->for_each_recursively([&](Data::ModelIndexInfoSet::flat_iterator infoIt)
    {
        ModelIndexInfo& info = *infoIt;

        // calculations only for dirty ones
        if (info.getRect().isNull())
        {
            QRect rect = calcItemRect(infoIt);
            info.setRect(rect);                  // size muse be set at this point, as children calculations may require it
        }

        if (info.getOverallRect().isNull())
        {
            QRect rect = info.getRect();

            //calculate overall only if node is expanded and has any children
            if (infoIt.children_count() != 0 && m_data->isExpanded(infoIt))
                for(Data::ModelIndexInfoSet::flat_iterator c_infoIt = infoIt.begin(); c_infoIt.valid(); ++c_infoIt)
                {
                    const ModelIndexInfo& c_info = *c_infoIt;
                    QRect c_rect = c_info.getOverallRect();
                    assert(c_rect.isValid());

                    rect = rect.united(c_rect);
                }

            info.setOverallRect(rect);
        }
    });
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
    const QRect items_pos = info.getOverallRect();
    assert(items_pos.isValid());
    const QPoint result = QPoint(0, items_pos.bottom());

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
    const QRect& indexRect = indexInfo.getRect();

    while(searchIt.index() > 0)
    {
        //go to previous item
        --searchIt;

        const QRect& prevRect = searchIt->getRect();

        if (prevRect.top() == indexRect.top())   //items are at the same y-position? If no - we are no longer in the same row
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
