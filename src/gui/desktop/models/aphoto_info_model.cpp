/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "aphoto_info_model.hpp"


Q_DECLARE_METATYPE(PhotoPropertiesObject)


PhotoPropertiesObject::PhotoPropertiesObject()
    : m_path()
    , m_height(0)
    , m_width(0)
{

}


PhotoPropertiesObject::PhotoPropertiesObject(const QString& path, int h, int w)
    : m_path(path)
    , m_height(h)
    , m_width(w)
{

}



APhotoInfoModel::APhotoInfoModel(QObject* p)
    : QAbstractItemModel(p)
{
    registerRole(PhotoProperties, "photoProperties");
}


APhotoInfoModel::~APhotoInfoModel()
{

}


QHash<int, QByteArray> APhotoInfoModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
    result.unite(m_customRoles);

    return result;
}


void APhotoInfoModel::registerRole(int id, const QByteArray& name)
{
    m_customRoles[id] = name;
}
