/*
    Helper class.
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


#include "image_manager.hpp"

#include <QPixmap>
#include <QPainter>
#include <QModelIndex>

//TODO: remove, use config
const int leftMargin = 20;
const int rightMargin = 20;
const int topMargin  = 20;
const int imageMargin = 10;
//


ImageManager::ImageManager(QAbstractItemModel* model) : m_model(model) {}


ImageManager::~ImageManager() {}


QSize ImageManager::size(int i) const
{
    QPixmap image(getPixmap(i));

    //image size
    QSize imageSize = image.size();

    //add margins
    imageSize.rwidth() += imageMargin * 2;
    imageSize.rheight() += imageMargin * 2;

    return imageSize;
}


void ImageManager::draw(int i, QPainter* painter, const QRect& rect) const
{
    QPixmap image(getPixmap(i));

    //image size
    QSize imageSize = image.size();

    QPoint center = rect.center();

    QRect target(center.x() - imageSize.width() / 2,
                 center.y() - imageSize.height() / 2,
                 imageSize.width(),
                 imageSize.height());

    painter->drawPixmap(target, image);
}


QPixmap ImageManager::getPixmap(int i) const
{
    QModelIndex index = m_model->index(i, 0);
    QVariant variant = m_model->data(index, Qt::DecorationRole);

    QPixmap image = variant.value<QPixmap>();

    return image;
}
