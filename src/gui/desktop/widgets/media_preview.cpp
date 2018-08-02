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

#include <cassert>

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
                m_label(p),
                m_path(path)
            {
                scale(1.0);
            }

            QWidget* getWidget() override
            {
                return &m_label;
            }

            void scale(double f) override
            {
                QPixmap pixmap(m_path);
                QSizeF size = pixmap.size();
                size *= f;

                auto scaled = pixmap.scaled(size.width(), size.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                m_label.setPixmap(scaled);
            }

        private:
            QLabel m_label;
            QString m_path;
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

            QWidget* getWidget() override
            {
                return &m_label;
            }

            void scale(double f) override
            {
                if (m_baseSize.isValid() == false)
                    m_baseSize = m_movie.frameRect().size();

                QSizeF size = m_baseSize;
                size *= f;

                m_movie.setScaledSize(size.toSize());
            }

        private:
            QLabel m_label;
            QMovie m_movie;
            QSize m_baseSize;
    };
}


MediaPreview::MediaPreview(QWidget* p):
    QWidget(p),
    m_interior(nullptr)
{
    new QHBoxLayout(this);
}


MediaPreview::~MediaPreview()
{
    delete m_interior;
}


void MediaPreview::clean()
{
    setMedia("");
}


void MediaPreview::setMedia(const QString& path)
{
    QLayout* l = layout();

    delete m_interior;
    m_interior = nullptr;

    if (path.isEmpty())
    {

    }
    else if (MediaTypes::isAnimatedImageFile(path) ||
             MediaTypes::isVideoFile(path))
    {
        AnimatedInternal* interior = new AnimatedInternal(path, this);
        m_interior = interior;

        QWidget* w = interior->getWidget();
        l->addWidget(w);
    }
    else if (MediaTypes::isImageFile(path))
    {
        StaticInternal* interior = new StaticInternal(path, this);
        m_interior = interior;

        QWidget* w = interior->getWidget();
        l->addWidget(w);
    }
    else
        assert(!"unknown file type");
}


void MediaPreview::scale(double f)
{
    m_interior->scale(f);
}
