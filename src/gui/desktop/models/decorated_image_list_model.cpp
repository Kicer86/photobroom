/*
 * Extension for ImageListModel which will gather extra info about inserted photo paths
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

#include "decorated_image_list_model.hpp"

#include <database/idatabase.hpp>

#include "utils/path_checker.hpp"


DecoratedImageListModel::DecoratedImageListModel(QObject* p): ImageListModel(p), m_pathChecker(new PathChecker), m_in_db(), m_in_db_mutex()
{
    connect(m_pathChecker.get(), &PathChecker::fileChecked, this, &DecoratedImageListModel::gotPathInfo);
}


DecoratedImageListModel::~DecoratedImageListModel()
{

}


void DecoratedImageListModel::insert(const QString& path)
{
    ImageListModel::insert(path);

    m_pathChecker->checkFile(path);
}


void DecoratedImageListModel::setDatabase(Database::IDatabase* db)
{
    m_pathChecker->set(db);
    
    auto notifier = db->notifier();
    connect(notifier, &Database::ADatabaseSignals::photoAdded, this, &DecoratedImageListModel::photoAdded);
    connect(notifier, &Database::ADatabaseSignals::photosRemoved, this, &DecoratedImageListModel::photosRemoved);
}


QVariant DecoratedImageListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (role == InDatabaseRole)
    {
        const QString path = get(index);

        std::lock_guard<std::mutex> lock(m_in_db_mutex);
        auto it = m_in_db.find(path);

        if (it == m_in_db.end())
        {
            result = ExistsInDatabase::DontKnowYet;
            m_pathChecker->checkFile(path);
        }
        else
            result = it->id.valid()? ExistsInDatabase::Yes: ExistsInDatabase::No;
    }
    else
        result = ImageListModel::data(index, role);

    return result;
}


Qt::ItemFlags DecoratedImageListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    const QString path = get(index);

    std::lock_guard<std::mutex> lock(m_in_db_mutex);
    auto f_it = m_in_db.find(path);

    assert(f_it != m_in_db.end());
    const bool exists = f_it->id.valid();

    if (exists)
        flags = flags & ~Qt::ItemIsSelectable;

    return flags;
}


void DecoratedImageListModel::photoChanged(const IPhotoInfo::Ptr& photoInfo)
{
    photoChanged(photoInfo->getPath());
}


void DecoratedImageListModel::photoChanged(const QString& path)
{
    const QModelIndex idx = get(path);

    if (idx.isValid())
        emit dataChanged(idx, idx, {InDatabaseRole} );
}


void DecoratedImageListModel::gotPathInfo(const QString& path, const Photo::Id& id)
{
    std::lock_guard<std::mutex> lock(m_in_db_mutex);

    PathInfo info = {path, id};

    m_in_db.insert(info);
}


void DecoratedImageListModel::photoAdded(const IPhotoInfo::Ptr& photoInfo)
{
    std::lock_guard<std::mutex> lock(m_in_db_mutex);

    PathInfo info = {photoInfo->getPath(), photoInfo->getID()};

    m_in_db.insert(info);

    photoChanged(photoInfo);
}


void DecoratedImageListModel::photosRemoved(const std::deque<Photo::Id>& photos)
{
    for(const Photo::Id& photoId: photos)
        photoRemoved(photoId);
}


void DecoratedImageListModel::photoRemoved(const Photo::Id& id)
{
    auto it = m_in_db.get<1>().find(id);
    const QString path = it->path;

    PathInfo info = { path, Photo::Id() };
    m_in_db.get<1>().replace(it,info);

    photoChanged(path);
}
