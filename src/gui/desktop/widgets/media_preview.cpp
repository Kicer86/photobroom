/*
 * Universal widget for media preview
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

#include "media_preview.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include <core/media_types.hpp>


namespace
{
    class StaticInternal: public MediaPreview::IInternal
    {
        public:
            StaticInternal(const QString& path, QWidget* p):
                m_label(p)
            {
                QPixmap pixmap(path);
                m_label.setPixmap(pixmap);
            }

            QWidget* getWidget()
            {
                return &m_label;
            }

        private:
            QLabel m_label;
    };


    class AnimatedInternal: public MediaPreview::IInternal
    {
        public:
            AnimatedInternal(const QString& path, QWidget* p):
                m_label(p),
                m_movie(path)
            {
                m_label.setMovie(&m_movie);
                m_movie.start();
            }

            QWidget* getWidget()
            {
                return &m_label;
            }

        private:
            QLabel m_label;
            QMovie m_movie;
    };
}


MediaPreview::MediaPreview(QWidget* p):
    QWidget(p),
    m_interior(nullptr)
{
    setLayout(new QHBoxLayout);
}


MediaPreview::~MediaPreview()
{
    delete m_interior;
}


void MediaPreview::setMedia(const QString& path)
{
    QLayout* l = layout();

    delete m_interior;
    m_interior = nullptr;

    if (MediaTypes::isImageFile(path))
    {
        StaticInternal* interior = new StaticInternal(path, this);
        m_interior = interior;

        QWidget* w = interior->getWidget();
        l->addWidget(w);
    }
    else if (MediaTypes::isAnimatedImageFile(path) ||
             MediaTypes::isVideoFile(path))
    {
        AnimatedInternal* interior = new AnimatedInternal(path, this);
        m_interior = interior;

        QWidget* w = interior->getWidget();
        l->addWidget(w);
    }
    else
        assert(!"unknown file type");
}
