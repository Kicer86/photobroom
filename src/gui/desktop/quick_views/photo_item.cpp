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

#include "photo_item.hpp"

#include <QPainter>

#include <core/signal_postponer.hpp>


using namespace std::placeholders;

PhotoItem::PhotoItem(QQuickItem* parent)
    : AMediaItem(parent)
    , m_thbMgr(nullptr)
{
    using namespace std::chrono_literals;
    lazy_connect(this, &QQuickPaintedItem::widthChanged, this, &PhotoItem::refetch, 1000ms, 5000ms);
}


void PhotoItem::paint(QPainter* painter)
{
    if (m_thbMgr == nullptr || source().valid() == false)
        return;

    if (state() == State::NotFetched)
        fetchImage();

    if (m_image.isNull() == false)
        paintImage(*painter);
}


void PhotoItem::setThumbnailsManager(IThumbnailsManager* mgr)
{
    m_thbMgr = mgr;
}


IThumbnailsManager* PhotoItem::thumbnailsManager() const
{
    return m_thbMgr;
}


void PhotoItem::updateThumbnail(const QImage& image)
{
    setImage(image);
    setState(State::Fetched);
    update();
}


void PhotoItem::setImage(const QImage& image)
{
    if (image.isNull())
        m_image.load(":/gui/error.svg");
    else
        m_image = image;
}


void PhotoItem::paintImage(QPainter& painter) const
{
    assert(m_image.isNull() == false);

    const QSize imgSize = m_image.size();
    const QRectF canvas(0.0, 0.0, width(), height());
    const QRectF photo(QPointF(0.0, 0.0), imgSize);

    QRectF photoPart = canvas;
    photoPart.moveCenter(photo.center());

    painter.drawImage(canvas.topLeft(), m_image, photoPart);
}


void PhotoItem::refetch()
{
    if (state() == State::Fetched && source().valid())
    {
        const QSize thbSize(width(), height());

        auto image = m_thbMgr->fetch(source(), thbSize);

        if (image.has_value())
            setImage(image.value());
        else
            m_thbMgr->fetch(source(), thbSize, queued_slot(this, &PhotoItem::updateThumbnail));
    }
}


void PhotoItem::fetchImage()
{
    const QSize thbSize(width(), height());

    auto image = m_thbMgr->fetch(source(), thbSize);

    if (image.has_value())
    {
        setImage(image.value());
        setState(State::Fetched);
    }
    else
    {
        setState(State::Fetching);

        m_thbMgr->fetch(source(), thbSize, queued_slot(this, &PhotoItem::updateThumbnail));
    }
}
