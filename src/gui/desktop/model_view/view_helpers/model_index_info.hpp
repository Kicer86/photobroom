/*
 * Model for view
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

#ifndef MODELINDEXINFO_HPP
#define MODELINDEXINFO_HPP

#include <QModelIndex>
#include <QRect>

#include <core/tree.hpp>

struct ModelIndexInfo
{
        QPersistentModelIndex index;
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


class ModelIndexInfoSet final
{
    public:
        typedef tree<ModelIndexInfo> Model;
        typedef tree_utils::recursive_iterator<Model::const_iterator> const_iterator;
        typedef tree_utils::recursive_iterator<Model::iterator>       iterator;

        ModelIndexInfoSet();
        ~ModelIndexInfoSet();

        const_iterator find(const QModelIndex &) const;
        const_iterator begin() const;
        const_iterator end() const;
        iterator find(const QModelIndex &);
        iterator begin();
        iterator end();

        void clear();
        void erase(const iterator &);
        void update(iterator &, const ModelIndexInfo &);

        bool empty() const;
        size_t size() const;

    private:
        Model m_model;
        
        std::vector<size_t> generateHierarchy(const QModelIndex &) const;
};

#endif // MODELINDEXINFO_H
