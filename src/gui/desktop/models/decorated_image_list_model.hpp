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

#ifndef DECORATEDIMAGELISTMODEL_HPP
#define DECORATEDIMAGELISTMODEL_HPP

#include <memory>
#include <mutex>

#include <database/iphoto_info.hpp>

#include "image_list_model.hpp"


namespace Database
{
    struct IDatabase;
}

class PathChecker;

class DecoratedImageListModel: public ImageListModel
{
    public:
        DecoratedImageListModel(QObject *);
        DecoratedImageListModel(const DecoratedImageListModel &) = delete;
        ~DecoratedImageListModel();
        DecoratedImageListModel& operator=(const DecoratedImageListModel &) = delete;

        void insert(const QString &);
        void setDatabase(Database::IDatabase *);

        // QAbstractItemModel interface:
        QVariant data(const QModelIndex& index, int role) const override;

        // extra roles
        enum ExistsInDatabase { No, Yes, DontKnowYet };

        static constexpr int InDatabaseRole = Qt::UserRole + 1;

    private:
        std::unique_ptr<PathChecker> m_pathChecker;
        std::map<QString, bool> m_in_db;
        mutable std::mutex m_in_db_mutex;

        void gotPathInfo(const QString &, bool);

        void photoAdded(const IPhotoInfo::Ptr &);
        void photoModified(const IPhotoInfo::Ptr &);
        void photosRemoved(const std::deque<IPhotoInfo::Ptr> &);
        void photoRemoved(const IPhotoInfo::Ptr &);
};

#endif // DECORATEDIMAGELISTMODEL_HPP
