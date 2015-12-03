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
            result = it->second? ExistsInDatabase::Yes: ExistsInDatabase::No;
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
    const bool exists = f_it->second;

    if (exists)
        flags = flags & ~Qt::ItemIsSelectable;

    return flags;
}


void DecoratedImageListModel::photoChanged(const IPhotoInfo::Ptr& photoInfo)
{
    const QModelIndex idx = get(photoInfo->getPath());

    if (idx.isValid())
        emit dataChanged(idx, idx, {InDatabaseRole} );
}


void DecoratedImageListModel::gotPathInfo(const QString& path, bool exists)
{
    std::lock_guard<std::mutex> lock(m_in_db_mutex);
    m_in_db[path] = exists;
}


void DecoratedImageListModel::photoAdded(const IPhotoInfo::Ptr& photoInfo)
{
    const QString path = photoInfo->getPath();

    m_in_db[path] = true;

    photoChanged(photoInfo);
}


void DecoratedImageListModel::photosRemoved(const std::deque<IPhotoInfo::Ptr>& photos)
{
    for(const IPhotoInfo::Ptr& photoInfo: photos)
        photoRemoved(photoInfo);
}


void DecoratedImageListModel::photoRemoved(const IPhotoInfo::Ptr& photoInfo)
{
    const QString path = photoInfo->getPath();

    m_in_db[path] = false;

    photoChanged(photoInfo);
}
