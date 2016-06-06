/*
 * Widget for photo preview
 * Copyright (C) 2016  Michał Walenciak <MichalWalenciak@gmail.com>
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


#include "photo_preview_widget.hpp"

#include <QLabel>

#include <core/iphotos_manager.hpp>


PhotoPreviewWidget::PhotoPreviewWidget(QWidget* parent): QScrollArea(parent), m_label(nullptr), m_manager(nullptr)
{
    m_label = new QLabel(this);
    setWidget(m_label);
    setWidgetResizable(true);
}


PhotoPreviewWidget::~PhotoPreviewWidget()
{

}


void PhotoPreviewWidget::set(IPhotosManager* manager)
{
    m_manager = manager;
}


void PhotoPreviewWidget::show(IPhotoInfo* photoInfo)
{
    const QString path = photoInfo->getPath();
    const QByteArray img_raw = m_manager->getPhoto(path);
    const QImage img = QImage::fromData(img_raw);
    const QPixmap pixmap = QPixmap::fromImage(img);

    m_label->setPixmap(pixmap);
}
