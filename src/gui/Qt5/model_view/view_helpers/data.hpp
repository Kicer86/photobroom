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
#include <deque>

#include <QRect>
#include <QModelIndex>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

struct IConfiguration;

struct ModelIndexInfo
{
    QModelIndex index;
    bool expanded;

    void setRect(const QRect& r);
    void setOverallRect(const QRect& r);
    const QRect& getRect() const;
    const QRect& getOverallRect() const;
    void cleanRects();

    ModelIndexInfo(const QModelIndex& idx = QModelIndex());

private:
    QRect rect;
    QRect overallRect;
};


class Data
{
    public:
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
                boost::multi_index::ordered_non_unique<boost::multi_index::const_mem_fun<ModelIndexInfo, const QRect &, &ModelIndexInfo::getRect>, QRectCompare>
            >
        > ModelIndexInfoSet;

        const int indexMargin = 10;           // TODO: move to configuration
        IConfiguration* m_configuration;

        Data(): m_configuration(nullptr), m_itemData(), m_invalid() {}
        Data(const Data &) = delete;
        Data& operator=(const Data &) = delete;

        ModelIndexInfo get(const QModelIndex &) const;
        void forget(const QModelIndex &);                    //clear data about given index

        const ModelIndexInfo& get(const QPoint &) const;
        bool isImage(const QModelIndex &) const;
        QPixmap getImage(const QModelIndex &) const;
        void for_each(std::function<bool(const ModelIndexInfo &)>) const;
        void for_each_visible(std::function<bool(const ModelIndexInfo &)>) const;

        bool isExpanded(const QModelIndex &) const;
        bool isVisible(const QModelIndex &) const;
        void for_each_recursively(QAbstractItemModel *, std::function<void(const QModelIndex &, const std::deque<QModelIndex> &)>, const QModelIndex& first = QModelIndex());
        void update(const ModelIndexInfo &);
        void clear();

        const ModelIndexInfoSet& getAll() const;

    private:
        ModelIndexInfoSet m_itemData;
        ModelIndexInfo m_invalid;

        std::deque<QModelIndex> for_each_recursively(QAbstractItemModel *, const QModelIndex &, std::function<void(const QModelIndex &, const std::deque<QModelIndex> &)>);
        void dump();
};

#endif // DATA_HPP
