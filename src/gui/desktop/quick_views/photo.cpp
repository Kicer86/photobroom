/*
 * Copyright (C) 2020  Michał Walenciak <Kicer86@gmail.com>
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
 */

#include "photo.hpp"

#include <QPainter>


PhotoItem::PhotoItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , m_source()
    , m_thbMgr(nullptr)
{

}


PhotoItem::~PhotoItem()
{

}


void PhotoItem::paint(QPainter *painter)
{
    if (m_thbMgr == nullptr)
        return;

    const int h = static_cast<int>(height());
    auto image = m_thbMgr->fetch(m_source, h);

    if (image.has_value())
        painter->drawImage(0, 0, image.value());
    else
        m_thbMgr->fetch(m_source, h, std::bind(&QQuickPaintedItem::update, this, QRect()));
}


void PhotoItem::setThumbnailsManager(IThumbnailsManager* mgr)
{
    m_thbMgr = mgr;
}


void PhotoItem::setSource(const QString& source)
{
    m_source = source;
}


IThumbnailsManager* PhotoItem::thumbnailsManager() const
{
    return m_thbMgr;
}


QString PhotoItem::source() const
{
    return m_source;
}
