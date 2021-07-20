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
#include <QImage>

#include <core/function_wrappers.hpp>
#include "utils/ithumbnails_manager.hpp"


class PhotoItem: public QQuickPaintedItem
{
        Q_OBJECT
        Q_PROPERTY(IThumbnailsManager* thumbnails WRITE setThumbnailsManager READ thumbnailsManager)
        Q_PROPERTY(Photo::Id photoID WRITE setSource READ source)
        Q_PROPERTY(State state READ state NOTIFY stateChanged)
        Q_ENUMS(State)

    public:
        enum class State
        {
            NotFetched,
            Fetching,
            Fetched
        };

        PhotoItem(QQuickItem *parent = nullptr);
        ~PhotoItem() = default;

        void paint(QPainter *painter) override;

        void setThumbnailsManager(IThumbnailsManager *);
        void setSource(const Photo::Id &);

        IThumbnailsManager* thumbnailsManager() const;
        const Photo::Id& source() const;
        State state() const;

    private:
        QImage m_image;
        IThumbnailsManager* m_thbMgr;
        State m_state;
        Photo::Id m_id;

        void updateThumbnail(const QImage &);
        void fetchImage();
        void setImage(const QImage &);
        void setState(State);
        void paintImage(QPainter &) const;
        void refetch();

    signals:
        void stateChanged();
};


#endif // PHOTOITEM_H
