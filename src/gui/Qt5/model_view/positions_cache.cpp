/*
    Helper class for Images View.
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "positions_cache.hpp"

#include <assert.h>

#include <QScrollBar>
#include <QAbstractItemView>

#include "image_manager.hpp"

struct PositionsCache::MutableData
{
    MutableData(QAbstractItemView* view): m_valid(false), m_pos(), m_rows(), m_view(view), m_totalHeight(0) {}
    MutableData(const MutableData &) = delete;
    void operator=(const MutableData &) = delete;

    bool m_valid;
    std::vector<QRect> m_pos;         //position of items on grid
    std::vector<int>   m_rows;        //each row's height
    QAbstractItemView *m_view;
    int                m_totalHeight;
};



PositionsCache::PositionsCache(QAbstractItemView* view) : m_data(new MutableData(view)) {}


PositionsCache::~PositionsCache() {}


void PositionsCache::invalidate() const
{
    m_data->m_valid = false;
    m_data->m_view->viewport()->update();
}


size_t PositionsCache::items() const
{
    validateCache();

    return m_data->m_pos.size();
}


QRect PositionsCache::pos(int i) const
{
    validateCache();
    const int verticalOffset = m_data->m_view->verticalScrollBar()->value();

    QRect rect = m_data->m_pos[i];
    rect.moveTo(rect.x(), rect.y() - verticalOffset );

    return rect;
}


void PositionsCache::flushData() const
{
    m_data->m_pos.clear();
    m_data->m_rows.clear();
    m_data->m_totalHeight = 0;
}


void PositionsCache::validateCache() const
{
    if (m_data->m_valid == false)
    {
        reloadCache();
        m_data->m_valid = true;
    }
}


void PositionsCache::reloadCache() const
{
    QAbstractItemModel* dataModel = m_data->m_view->model();

    if (dataModel != nullptr)
    {
        const int baseX = 0;
        const int width = m_data->m_view->viewport()->width();
        int x = baseX;
        int y = 0;
        int rowHeight = 0;

        flushData();
        const int count = dataModel->rowCount(QModelIndex());

        ImageManager imageManager(dataModel);

        for(int i = 0; i < count; i++)
        {
            //image size
            QSize size = imageManager.size(i);

            //check if position is correct
            if (x + size.width() >= width)      //no place? go to next row
            {
                assert(rowHeight > 0);
                x = baseX;
                y += rowHeight;

                m_data->m_rows.push_back(rowHeight);
                m_data->m_totalHeight += rowHeight;
                rowHeight = 0;
            }

            //save position
            QRect position(x, y, size.width(), size.height());
            m_data->m_pos.push_back(position);

            x += size.width();

            rowHeight = std::max(rowHeight, size.height());
        }

        //save last row
        m_data->m_rows.push_back(rowHeight);
        m_data->m_totalHeight += rowHeight;

        //update scroll bars
        updateScrollBars();

    }
}


void PositionsCache::updateScrollBars() const
{
    QSize areaSize = m_data->m_view->viewport()->size();

    const int avail_height = areaSize.height();
    const int range_top = m_data->m_totalHeight - avail_height;

    m_data->m_view->verticalScrollBar()->setPageStep(avail_height);
    m_data->m_view->verticalScrollBar()->setRange(0, range_top);
}












