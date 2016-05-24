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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

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
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        // extra roles
        enum ExistsInDatabase { No, Yes, DontKnowYet };

        static constexpr int InDatabaseRole = Qt::UserRole + 1;

    private:
        struct PathInfo
        {
            QString   path;
            Photo::Id id;
        };

        typedef boost::multi_index::multi_index_container<
            PathInfo,
            boost::multi_index::indexed_by<
                boost::multi_index::ordered_unique<boost::multi_index::member<PathInfo, QString, &PathInfo::path> >,
                boost::multi_index::ordered_non_unique<boost::multi_index::member<PathInfo, Photo::Id, &PathInfo::id> >
            >
        > PathInfoContainer;

        std::unique_ptr<PathChecker> m_pathChecker;
        PathInfoContainer m_in_db;
        mutable std::mutex m_in_db_mutex;

        void photoChanged(const IPhotoInfo::Ptr &);
        void photoChanged(const QString &);

        void gotPathInfo(const QString &, const Photo::Id &);

        void photoAdded(const IPhotoInfo::Ptr &);
        void photosRemoved(const std::deque<Photo::Id> &);
        void photoRemoved(const Photo::Id &);
};

#endif // DECORATEDIMAGELISTMODEL_HPP
