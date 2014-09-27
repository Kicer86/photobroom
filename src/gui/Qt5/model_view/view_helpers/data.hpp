/*
 * View's data structure
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef DATA_HPP
#define DATA_HPP

#include <unordered_map>

#include <QRect>
#include <QModelIndex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

struct IConfiguration;

class Data
{
    public:
        struct ModelIndexInfo
        {
            QModelIndex index;
            QRect rect;
            bool expanded;
            bool visible;

            ModelIndexInfo(const QModelIndex& idx = QModelIndex()): index(idx), rect(), expanded(false), visible(false) {}
        };

        const int indexMargin = 10;           // TODO: move to configuration
        IConfiguration* m_configuration;

        Data(): m_configuration(nullptr), m_itemData() {}
        Data(const Data &) = delete;
        Data& operator=(const Data &) = delete;

        ModelIndexInfo get(const QModelIndex &);

        ModelIndexInfo get(const QPoint &) const;
        bool isImage(QAbstractItemModel *, const QModelIndex &) const;
        QPixmap getImage(QAbstractItemModel *, const QModelIndex &) const;
        void for_each(std::function<bool(const ModelIndexInfo &)>) const;

        void add(const ModelIndexInfo &);
        void clear();

    private:
        struct IndexHasher
        {
            std::size_t operator()(const QModelIndex& index) const
            {
                return reinterpret_cast<std::size_t>(index.internalPointer());
            }
        };

        struct QRectCompare
        {
            bool operator()(const QRect& r1, const QRect& r2) const
            {
                bool result = false;

                if (r1.y() < r2.y())
                    result = true;
                else if (r1.y() == r2.y())
                    result = r1.x() < r2.x();

                return result;
            }
        };

        typedef boost::multi_index_container
        <
            ModelIndexInfo,
            boost::multi_index::indexed_by
            <
                boost::multi_index::hashed_unique<boost::multi_index::member<ModelIndexInfo, QModelIndex, &ModelIndexInfo::index>, IndexHasher>,
                boost::multi_index::ordered_non_unique<boost::multi_index::member<ModelIndexInfo, QRect, &ModelIndexInfo::rect>, QRectCompare>
            >
        > ModelIndexInfoSet;

        ModelIndexInfoSet m_itemData;
};

#endif // DATA_HPP
