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

#include <core/function_wrappers.hpp>


PhotoItem::PhotoItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , m_image()
    , m_source()
    , m_thbMgr(nullptr)
    , m_state(State::NotFetched)
    , m_photoWidth(0)
    , m_photoHeight(0)
{

}


PhotoItem::~PhotoItem()
{

}


void PhotoItem::paint(QPainter *painter)
{
    if (m_thbMgr == nullptr || m_photoHeight == 0 || m_photoWidth == 0 || m_source.isEmpty())
        return;

    if (m_state == State::NotFetched)
        fetchImage();

    paintImage(*painter);
}


void PhotoItem::setThumbnailsManager(IThumbnailsManager* mgr)
{
    m_thbMgr = mgr;
}


void PhotoItem::setSource(const QString& source)
{
    m_source = source;
}


void PhotoItem::setPhotoWidth(int w)
{
    m_photoWidth = w;
}


void PhotoItem::setPhotoHeight(int h)
{
    m_photoHeight = h;
}


IThumbnailsManager* PhotoItem::thumbnailsManager() const
{
    return m_thbMgr;
}


QString PhotoItem::source() const
{
    return m_source;
}


int PhotoItem::photoWidth() const
{
    return m_photoWidth;
}


int PhotoItem::photoHeight() const
{
    return m_photoHeight;
}


void PhotoItem::gotThumbnail(const QImage& image)
{
    if (image.isNull())
        m_image.load(":/gui/error.svg");
    else
        m_image = image;

    m_state = State::Fetched;
    update();
}


void PhotoItem::paintImage(QPainter& painter) const
{
    assert(m_image.isNull() == false);

    const QRectF canvas(0.0, 0.0, width(), height());
    const QRectF photo(QPointF(0.0, 0.0), m_image.size());

    QRectF photoPart = canvas;
    photoPart.moveCenter(photo.center());

    painter.drawImage(canvas.topLeft(), m_image, photoPart);
}


void PhotoItem::fetchImage()
{
    const QSize thbSize = calculateThumbnailSize();
    const int h = thbSize.height();

    auto image = m_thbMgr->fetch(m_source, h);

    if (image.has_value())
    {
        m_image = image.value();
        m_state = State::Fetched;
    }
    else
    {
        m_state = State::Fetching;
        m_image.load(":/gui/clock.svg");
        m_thbMgr->fetch(m_source, h, queued_slot<PhotoItem, void, const QImage &>(this, &PhotoItem::gotThumbnail));
    }
}


QSize PhotoItem::calculateThumbnailSize() const
{
    const int h = static_cast<int>(height());
    const int w = static_cast<int>(width());

    QSize thbSize(m_photoWidth, m_photoHeight);
    thbSize.scale(w, h, Qt::KeepAspectRatioByExpanding);

    return thbSize;
}
