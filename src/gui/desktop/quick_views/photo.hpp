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
#include <core/icore_factory_accessor.hpp>
#include <core/ithumbnails_manager.hpp>


class PhotoItem: public QQuickPaintedItem
{
        Q_OBJECT
        Q_PROPERTY(ICoreFactoryAccessor* core WRITE core READ getCore)
        Q_PROPERTY(IThumbnailsManager* thumbnails WRITE thumbnails READ getThumbnails)
        Q_PROPERTY(QString source WRITE setSource READ source)

    public:
        PhotoItem(QQuickItem *parent = nullptr);
        ~PhotoItem();

        void paint(QPainter *painter) override;

        void core(ICoreFactoryAccessor *);
        void thumbnails(IThumbnailsManager *);
        void setSource(const QString &);

        ICoreFactoryAccessor* getCore() const;
        IThumbnailsManager* getThumbnails() const;
        QString source() const;

    private:
        QString m_source;
        ICoreFactoryAccessor* m_core;
        IThumbnailsManager* m_thbMgr;
};

#endif // PHOTOITEM_H
