/*
    Data Model for Images.
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


#ifndef IMAGES_MODEL_HPP
#define IMAGES_MODEL_HPP

#include <QAbstractListModel>

#include "core/aphoto_info.hpp"
#include "Qt5/thread_multiplexer.hpp"

class PhotoInfo;

class ImagesModel: public QAbstractListModel, public IThreadMultiplexer<PhotoInfo *>::IGetter
{
    public:
        ImagesModel();
        virtual ~ImagesModel();

        void add(const APhotoInfo::Ptr &);

        APhotoInfo::Ptr get(const QModelIndex &);
        const std::vector<APhotoInfo::Ptr>& getAll() const;

        //QAbstractItemModel:

        //QAbstractListModel:
        int rowCount(const QModelIndex &/*parent*/) const;
        QVariant data(const QModelIndex &_index, int role) const;

        //IThreadMultiplexer::IGetter:
        virtual void getSignal(PhotoInfo * const &);

    private:
        std::vector<APhotoInfo::Ptr> m_photos;
};

#endif // IMAGES_MODEL_HPP
