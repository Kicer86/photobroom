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

#ifndef MEDIAPREVIEW_HPP
#define MEDIAPREVIEW_HPP


#include <QWidget>


class MediaPreview : public QWidget
{
    public:
        MediaPreview(QWidget *);
        ~MediaPreview();

        void setMedia(const QString &);
        void scale(double);

        struct IInternal
        {
            virtual ~IInternal() = default;

            virtual QWidget* getWidget() = 0;
            virtual void scale(double) = 0;
        };

    private:
        IInternal* m_interior;
};

#endif // MEDIAPREVIEW_HPP
