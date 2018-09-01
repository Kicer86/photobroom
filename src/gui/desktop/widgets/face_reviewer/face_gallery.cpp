/*
 * Widget displaying faces in horizontal line
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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

#include "face_gallery.hpp"

#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QScrollArea>


FaceGallery::FaceGallery(QWidget* p):
    QWidget(p),
    m_layout(new QHBoxLayout)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    QScrollArea* area = new QScrollArea(this);
    QWidget* canvas = new QWidget(this);

    canvas->setLayout(m_layout);

    area->setWidgetResizable(true);
    area->setWidget(canvas);
    area->setMinimumHeight(120);

    l->addWidget(area);
}


void FaceGallery::fill(const std::vector<QImage>& faces)
{
    for(const QImage& face: faces)
    {
        const QImage scaled = face.scaled(QSize(100, 100), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QPixmap scaledFace = QPixmap::fromImage(scaled);

        QLabel* label = new QLabel(this);
        label->setPixmap(scaledFace);

        m_layout->addWidget(label);
    }

    m_layout->addStretch();
}
