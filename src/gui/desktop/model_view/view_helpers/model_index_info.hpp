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
        bool expanded;

        void setRect(const QRect& r);
        void setOverallRect(const QRect& r);
        const QRect& getRect() const;
        const QRect& getOverallRect() const;
        void cleanRects();

        ModelIndexInfo();

    private:
        QRect rect;
        QRect overallRect;
};


class ModelIndexInfoSet final
{
    public:
        typedef tree<ModelIndexInfo> Model;
        typedef Model::const_iterator const_iterator;
        typedef Model::iterator       iterator;

        typedef Model::const_flat_iterator const_flat_iterator;
        typedef Model::flat_iterator       flat_iterator;

        ModelIndexInfoSet();
        ~ModelIndexInfoSet();

        const_iterator find(const QModelIndex &) const;
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cfind(const QModelIndex &) const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        iterator find(const QModelIndex &);
        iterator begin();
        iterator end();

        void clear();
        void erase(const iterator &);
        iterator insert(const QModelIndex &, const ModelIndexInfo &);

        bool empty() const;
        size_t size() const;

    private:
        Model m_model;
        
        std::vector<size_t> generateHierarchy(const QModelIndex &) const;
};

#endif // MODELINDEXINFO_H
