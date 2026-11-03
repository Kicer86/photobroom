/*
 * Photo Broom - photos management tool.
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
#include <QImageReader>
#include <QLabel>
#include <QMovie>

#include <core/media_types.hpp>


namespace
{
    class StaticInternal: public MediaPreview::IInternal
    {
        public:
            StaticInternal(const Filesystem::Location& path, QWidget* p):
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
                const auto image = Filesystem::openFile(m_path);
                QImageReader reader(image.get());
                const QPixmap pixmap = QPixmap::fromImage(reader.read());

                QSizeF size = pixmap.size();
                size *= f;

                auto scaled = pixmap.scaled(size.width(), size.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                m_label.setPixmap(scaled);
            }

        private:
            QLabel m_label;
            Filesystem::Location m_path;
    };


    class AnimatedInternal: public MediaPreview::IInternal
    {
        public:
            AnimatedInternal(const Filesystem::Location& path, QWidget* p)
                : m_label(p)
                , m_file(Filesystem::openFile(path))
                , m_movie(std::make_unique<QMovie>(m_file.get()))
            {
                m_label.setMovie(&*m_movie);
                m_movie->start();
            }

            QWidget* getWidget() override
            {
                return &m_label;
            }

            void scale(double f) override
            {
                if (m_baseSize.isValid() == false)
                    m_baseSize = m_movie->frameRect().size();

                QSizeF size = m_baseSize;
                size *= f;

                m_movie->setScaledSize(size.toSize());
            }

        private:
            QLabel m_label;
            QSize m_baseSize;
            std::unique_ptr<Filesystem::IFile> m_file;
            std::unique_ptr<QMovie> m_movie;
    };
}


MediaPreview::MediaPreview(QWidget* p):
    QWidget(p)
{
    new QHBoxLayout(this);
}


MediaPreview::~MediaPreview()
{

}


void MediaPreview::clean()
{
    setMedia({});
}


void MediaPreview::setMedia(const Filesystem::Location& location)
{
    QLayout* l = layout();

    m_interior.reset();

    if (location.isEmpty())
    {

    }
    else if (MediaTypes::isAnimatedImageFile(location) ||
             MediaTypes::isVideoFile(location))
    {
        m_interior = std::make_unique<AnimatedInternal>(location, this);

        QWidget* w = m_interior->getWidget();
        l->addWidget(w);
    }
    else if (MediaTypes::isImageFile(location))
    {
        m_interior = std::make_unique<StaticInternal>(location, this);

        QWidget* w = m_interior->getWidget();
        l->addWidget(w);
    }
    else
        assert(!"unknown file type");

    emit scalableContentAvailable(m_interior != nullptr);
}


void MediaPreview::scale(double f)
{
    assert(m_interior != nullptr);
    m_interior->scale(f);
}
