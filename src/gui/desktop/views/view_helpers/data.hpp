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

#include <deque>
#include <functional>

#include <QRect>
#include <QModelIndex>

#include "model_index_info.hpp"
#include "view_data_set.hpp"

struct IConfiguration;


class Data
{
    public:
        typedef ViewDataSet<ModelIndexInfo> ModelIndexInfoSet;

        Data();
        Data(const Data &) = delete;

        ~Data();
        Data& operator=(const Data &) = delete;

        void set(QAbstractItemModel *);
        [[deprecated]] void set(IConfiguration *);

        void setSpacing(int);
        void setThumbHeight(int);

        ModelIndexInfoSet::iterator get(const QModelIndex &) const;                 // Same as find(), but has assert inside. Use when result is not expeted to be invalid.
        ModelIndexInfoSet::const_iterator cfind(const QModelIndex &) const;
        ModelIndexInfoSet::iterator find(const QModelIndex &);

        ModelIndexInfoSet::iterator get(const QPoint &) const;
        bool isImage(const ModelIndexInfoSet::iterator &) const;
        QPixmap getImage(Data::ModelIndexInfoSet::level_iterator) const;
        QSize getThumbnailSize(Data::ModelIndexInfoSet::level_iterator) const;
        void for_each_visible(std::function<bool(ModelIndexInfoSet::iterator)>) const;
        QModelIndex get(const ModelIndexInfoSet::const_iterator &) const;
        std::deque<QModelIndex> findInRect(const QRect &) const;

        bool isExpanded(const ModelIndexInfoSet::const_iterator &) const;
        bool isVisible(const ModelIndexInfoSet::const_iterator &) const;

        const ModelIndexInfoSet& getModel() const;
        ModelIndexInfoSet& getModel();

        int getMargin() const;
        IConfiguration* getConfig();

        //getting siblings
        QModelIndex getRightOf(const QModelIndex &) const;
        QModelIndex getLeftOf(const QModelIndex &) const;
        QModelIndex getTopOf(const QModelIndex &) const;
        QModelIndex getBottomOf(const QModelIndex &) const;

        QModelIndex getFirst(const QModelIndex &) const;
        QModelIndex getLast(const QModelIndex &) const;

    private:
        std::unique_ptr<ModelIndexInfoSet> m_itemData;
        QAbstractItemModel* m_model;
        IConfiguration* m_configuration;
        int m_spacing;
        int m_thumbHeight;

        std::deque<QModelIndex> findInRect(ModelIndexInfoSet::const_level_iterator, ModelIndexInfoSet::const_level_iterator, const QRect &) const;
};

#endif // DATA_HPP
