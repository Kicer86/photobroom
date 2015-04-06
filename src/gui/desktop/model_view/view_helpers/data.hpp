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

        typedef ViewDataSet<ModelIndexInfo> ModelIndexInfoSet;

        const int indexMargin = 10;           // TODO: move to configuration
        IConfiguration* m_configuration;

        Data();
        Data(const Data &) = delete;
        
        ~Data();
        Data& operator=(const Data &) = delete;

        void set(QAbstractItemModel *);

        ModelIndexInfoSet::iterator get(const QModelIndex &) const;                 // Same as find(), but has assert inside. Use when result is not expeted to be invalid.
        ModelIndexInfoSet::const_iterator cfind(const QModelIndex &) const;
        ModelIndexInfoSet::iterator find(const QModelIndex &);
        
        ModelIndexInfoSet::iterator get(const QPoint &) const;
        bool isImage(const ModelIndexInfoSet::iterator &) const;
        QPixmap getImage(Data::ModelIndexInfoSet::flat_iterator) const;
        void for_each_visible(std::function<bool(ModelIndexInfoSet::iterator)>) const;
        QModelIndex get(const ModelIndexInfoSet::iterator &) const;

        bool isExpanded(const ModelIndexInfoSet::iterator &) const;
        bool isExpanded(const ModelIndexInfoSet::const_iterator &) const;
        bool isVisible(const ModelIndexInfoSet::iterator &) const;
        bool isVisible(const ModelIndexInfoSet::const_iterator &) const;
        void for_each_recursively(std::function<void(ModelIndexInfoSet::flat_iterator)>);

        const ModelIndexInfoSet& getModel() const;
        ModelIndexInfoSet& getModel();

    private:
        std::unique_ptr<ModelIndexInfoSet> m_itemData;
        QAbstractItemModel* m_model;

        void for_each_recursively(ModelIndexInfoSet::flat_iterator, std::function<void(ModelIndexInfoSet::flat_iterator)>);
};

#endif // DATA_HPP
