
/*
 * Plain list of IPHotoInfo
 * Copyright (C) 2016  Michał Walenciak <Kicer86@gmail.com>
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

#ifndef PHOTOINFOLISTMODEL_HPP
#define PHOTOINFOLISTMODEL_HPP

#include "aphoto_info_model.hpp"

class PhotoInfoListModel: public APhotoInfoModel
{
    public:
        PhotoInfoListModel();
        PhotoInfoListModel(const PhotoInfoListModel &) = delete;
        virtual ~PhotoInfoListModel();

        PhotoInfoListModel& operator=(const PhotoInfoListModel &) = delete;

        void set(const std::vector<IPhotoInfo::Ptr> &);

        IPhotoInfo* getPhotoInfo(const QModelIndex &) const override;

    private:
        std::vector<IPhotoInfo::Ptr> m_photos;
};

#endif // PHOTOINFOLISTMODEL_HPP
