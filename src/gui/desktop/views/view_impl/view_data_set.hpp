/*
 * Photo Broom - photos management tool.
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

#ifndef VIEW_DATA_SET_HPP
#define VIEW_DATA_SET_HPP

#include <cassert>
#include <iostream>

#include <QAbstractItemModel>
#include <QHash>
#include <QModelIndex>
#include <QRect>


template<typename T>
class ViewDataSet final
{
    public:
        ViewDataSet(): m_model(), m_db_model(nullptr)
        {
            clear();
        }

        ViewDataSet(const ViewDataSet<T> &) = delete;

        ~ViewDataSet()
        {
        }

        ViewDataSet<T>& operator=(const ViewDataSet<T> &) = delete;

        void set(QAbstractItemModel* model)
        {
            m_db_model = model;
        }

        T* find(const QModelIndex& index)
        {
            T* result = nullptr;

            auto it = m_model.find(getKey(index));

            if (it != m_model.end())
                result = &(it.value());

            return result;
        }

        template<typename F>
        void for_each(const F& f)
        {
            for(auto& item: m_model)
                f(item);
        }

        // to be called by view:
        void rowsInserted(const QModelIndex& parent, int from, int to)
        {
            for( int i = from; i <= to; i++)
            {
                const QModelIndex child_idx = m_db_model->index(i, 0, parent);
                insert(child_idx);

                //check if inserted item has children, and add them if any
                const int children = m_db_model->rowCount(child_idx);
                if (children)
                    rowsInserted(child_idx, 0, children - 1);
            }
        }

        void rowsAboutToBeRemoved(const QModelIndex& parent, int from , int to)
        {
            assert(m_removalInfo.empty());

            // collect data for future removal
            addItemsToRemovalList(parent, from, to);
        }

        void rowsRemoved(const QModelIndex &, int, int)
        {
            for (const Key& key: m_removalInfo)
            {
                auto k_it = m_model.find(key);
                assert(k_it != m_model.end());
                m_model.erase(k_it);
            }

            m_removalInfo.clear();
        }

        void modelReset()
        {
            clear();

            if (m_db_model != nullptr)
            {
                //load all data
                loadIndex(QModelIndex());
            }
        }

        bool validate() const
        {
            return true;
        }

        std::size_t size() const
        {
            return m_model.size();
        }

    private:
        typedef QPersistentModelIndex Key;
        std::vector<Key> m_removalInfo;

        QHash<Key, T> m_model;
        QAbstractItemModel* m_db_model;

        void clear()
        {
            m_model.clear();

            //add item for QModelIndex() which is always present
            insert(QModelIndex());
        }

        void insert(const QModelIndex& index)
        {
            const auto key = getKey(index);
            assert(m_model.find(key) == m_model.end());
            m_model.insert(key, T(index));
        }

        void loadIndex(const QModelIndex& p)
        {
            const int c = m_db_model->rowCount(p);

            for(int i = 0; i < c; i++)
            {
                const QModelIndex c_idx = m_db_model->index(i, 0, p);

                insert(c_idx);
                loadIndex(c_idx);
            }
        }

        Key getKey(const QModelIndex& idx) const
        {
            return QPersistentModelIndex(idx);
        }

        void addItemsToRemovalList(const QModelIndex& parent, int from, int to)
        {
            std::vector<Key> keys;
            keys.reserve(to - from + 1);

            for(int i = from; i <= to; i++)
            {
                const QModelIndex idx = m_db_model->index(i, 0, parent);
                const auto key = getKey(idx);

                keys.push_back(key);

                const int children = m_db_model->rowCount(idx);

                if (children > 0)
                    addItemsToRemovalList(idx, 0, children - 1);
            }

            m_removalInfo.insert(m_removalInfo.end(), keys.cbegin(), keys.cend());
        }
};


template<typename Target>
void connectModelAndView(QAbstractItemModel* model, Target* target)
{
    QObject::connect(model, &QAbstractItemModel::rowsInserted, target, &Target::rowsInserted);
    QObject::connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, target, &Target::rowsAboutToBeRemoved);
    QObject::connect(model, &QAbstractItemModel::modelReset, target, &Target::modelReset);

    target->modelReset();
}


#endif // VIEW_DATA_SET_HPP
