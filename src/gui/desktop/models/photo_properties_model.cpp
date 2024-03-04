/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2017  Michał Walenciak <Kicer86@gmail.com>
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


#include "photo_properties_model.hpp"

#include <algorithm>
#include <QDir>
#include <QFileInfo>


import system;

#include "gui/desktop/utils/photo_delta_fetcher_binding.hpp"


namespace
{
    QString geometryToStr(const QSize& geometry)
    {
        const QString result = geometry.isValid()?
            QObject::tr("%1×%2").arg(geometry.width()).arg(geometry.height()):
            QObject::tr("?");

        return result;
    }

    QString geometryToStr(const PhotoPropertiesModel::PhotoDelta& photoInfo)
    {
        return geometryToStr(photoInfo.get<Photo::Field::Geometry>());
    }

    QString cutPrj(const QString& path)
    {
        const QString result = path.left(5) == "prj:/"? path.mid(5): path;

        return result;
    }
}


PhotoPropertiesModel::PhotoPropertiesModel(QObject* p):
    QStandardItemModel(p)
    , m_fetcher(*this, &PhotoPropertiesModel::gotPhotoData)
{

}


PhotoPropertiesModel::~PhotoPropertiesModel()
{

}


void PhotoPropertiesModel::setDatabase(Database::IDatabase* db)
{
    m_db = db;

    m_fetcher.setDatabase(m_db);
}


void PhotoPropertiesModel::setPhotos(const std::vector<Photo::Id>& ids)
{
    if (m_db)
    {
        setBusy(true);
        m_fetcher.fetchIds(ids, {Photo::Field::Path, Photo::Field::Geometry});
    }
}


Database::IDatabase* PhotoPropertiesModel::database() const
{
    return m_db;
}


bool PhotoPropertiesModel::busy() const
{
    return m_busy;
}


void PhotoPropertiesModel::gotPhotoData(const std::vector<Photo::DataDelta>& data)
{
    const auto explicitData = Photo::EDV<PhotoDelta>(data);

    refreshLabels(explicitData);
    refreshValues(explicitData);

    setBusy(false);
}


void PhotoPropertiesModel::refreshLabels(const std::vector<PhotoDelta>& photos)
{
    const int s = static_cast<int>(photos.size());

    QStandardItem* locItem  = new QStandardItem(tr("Photo(s) location:", "", s));
    QStandardItem* sizeItem = new QStandardItem(tr("Photo(s) size:", "", s));
    QStandardItem* geomItem = new QStandardItem(tr("Photo(s) geometry:", "", s));

    setItem(0, 0, locItem);
    setItem(1, 0, sizeItem);
    setItem(2, 0, geomItem);
}


void PhotoPropertiesModel::refreshValues(const std::vector<PhotoDelta>& photos)
{
    const std::size_t s = photos.size();

    QStandardItem* locItem  = new QStandardItem;
    QStandardItem* sizeItem = new QStandardItem;
    QStandardItem* geomItem = new QStandardItem;

    // calculate photos size
    std::size_t size = 0;
    for(std::size_t i = 0; i < s; i++)
    {
        const QFileInfo info(photos[i].get<Photo::Field::Path>());
        size += info.size();
    }

    const QString size_human = sizeHuman(size);

    // handle various cases
    if (s == 0)
    {
        locItem->setText("---");
        sizeItem->setText("---");
        geomItem->setText("---");
    }
    else if (s == 1)
    {
        const PhotoDelta& photo = photos.front();
        const QString& filePath = photo.get<Photo::Field::Path>();
        const QString geometry = geometryToStr(photo);

        // update values
        locItem->setText(cutPrj(filePath));
        sizeItem->setText(size_human);
        geomItem->setText(geometry);
    }
    else
    {
        // 'merge' paths
        QString result = photos.front().get<Photo::Field::Path>();

        for(std::size_t i = 1; i < s; i++)
        {
            const QString anotherPhotoPath = photos[i].get<Photo::Field::Path>();
            result = FileSystem().commonPath(result, anotherPhotoPath);
        }

        const QString decorated_path = result + (result.right(1) == QDir::separator()? QString("") : QDir::separator()) + "...";

        // try to merge geometry
        const QSize geometry = photos.front().get<Photo::Field::Geometry>();
        const bool equal = std::all_of(photos.cbegin(), photos.cend(), [&geometry](const PhotoDelta& photo)
        {
            return photo.get<Photo::Field::Geometry>() == geometry;
        });

        const QString geometryStr = equal? geometryToStr(geometry): "---";

        // update values
        locItem->setText(cutPrj(decorated_path));
        sizeItem->setText(size_human);
        geomItem->setText(geometryStr);
    }

    setItem(0, 1, locItem);
    setItem(1, 1, sizeItem);
    setItem(2, 1, geomItem);
}


QString PhotoPropertiesModel::sizeHuman(qint64 size) const
{
    int i = 0;
    for(; i < 4 && size > 20480; i++)
        size /= 1024;

    const int sh = static_cast<int>(size);

    QString units;
    switch (i)
    {
        case 0:  units = tr("%n byte(s)",  "", sh); break;
        case 1:  units = tr("%n kbyte(s)", "", sh); break;
        case 2:  units = tr("%n Mbyte(s)", "", sh); break;
        default: units = tr("%n Gbyte(s)", "", sh); break;
    }

    return units;
}


void PhotoPropertiesModel::setBusy(bool b)
{
    m_busy = b;

    emit busyChanged(b);
}
