
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


#include "photo_info_list_model.hpp"


PhotoInfoListModel::PhotoInfoListModel( )
{

}


PhotoInfoListModel::~PhotoInfoListModel()
{

}


IPhotoInfo* PhotoInfoListModel::getPhotoInfo(const QModelIndex& index) const
{
    IPhotoInfo* result = nullptr;

    const QModelIndex parent = index.parent();

    if (parent.isValid() == false)
    {
        assert(index.column() == 0);
        assert(index.row() < m_photos.size());

        result = m_photos[index.row()].get();
    }

    return result;
}


void PhotoInfoListModel::set(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_photos = photos;
}
