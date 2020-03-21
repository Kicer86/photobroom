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

#ifndef PHOTOITEM_HPP
#define PHOTOITEM_HPP

#include <QQuickPaintedItem>
#include <core/ithumbnails_manager.hpp>


class PhotoItem: public QQuickPaintedItem
{
        Q_OBJECT
        Q_PROPERTY(IThumbnailsManager* thumbnails WRITE setThumbnailsManager READ thumbnailsManager)
        Q_PROPERTY(QString source WRITE setSource READ source)
        Q_PROPERTY(int photoWidth WRITE setPhotoWidth READ photoWidth)
        Q_PROPERTY(int photoHeight WRITE setPhotoHeight READ photoHeight)

    public:
        PhotoItem(QQuickItem *parent = nullptr);
        ~PhotoItem();

        void paint(QPainter *painter) override;

        void setThumbnailsManager(IThumbnailsManager *);
        void setSource(const QString &);
        void setPhotoWidth(int);
        void setPhotoHeight(int);

        IThumbnailsManager* thumbnailsManager() const;
        QString source() const;
        int photoWidth() const;
        int photoHeight() const;

    private:
        QString m_source;
        int m_photoWidth;
        int m_photoHeight;
        IThumbnailsManager* m_thbMgr;

        void gotThumbnail(const QImage &);
};


#endif // PHOTOITEM_H
