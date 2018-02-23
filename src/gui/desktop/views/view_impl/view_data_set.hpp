/*
 * View's low level data structure
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

struct IViewDataSet
{
    virtual ~IViewDataSet();

    virtual void rowsInserted(const QModelIndex &, int, int) = 0;
    virtual void rowsAboutToBeRemoved(const QModelIndex &, int, int) = 0;
    virtual void modelReset() = 0;
};


namespace ViewData
{
    inline quintptr constructId(const QModelIndex& idx)
    {
        return idx.internalId();
    }

    inline QPersistentModelIndex constructPersistent(const QModelIndex& idx)
    {
        return QPersistentModelIndex(idx);
    }
}


template<typename T, typename Key, Key (*Constructor)(const QModelIndex &)>
class ViewDataSet final: public IViewDataSet
{

    public:
        typedef QHash<Key, T>  Model;

        ViewDataSet(): m_model(), m_db_model(nullptr)
        {
            clear();
        }

        ViewDataSet(const ViewDataSet<T, Key, Constructor> &) = delete;

        ~ViewDataSet()
        {
        }

        ViewDataSet<T, Key, Constructor>& operator=(const ViewDataSet<T, Key, Constructor> &) = delete;

        void set(QAbstractItemModel* model)
        {
            m_db_model = model;
        }

        const T* find(const QModelIndex& index) const
        {
            T* result = nullptr;

            auto it = m_model.find(getKey(index));

            if (it != m_model.end())
                result = &(*it);

            return result;
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
        void rowsInserted(const QModelIndex& parent, int from, int to) override
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

        void rowsAboutToBeRemoved(const QModelIndex& parent, int from , int to) override
        {
            //update model
            for(int i = from; i <= to; i++)
            {
                const QModelIndex idx = m_db_model->index(i, 0, parent);
                erase(idx);
            }
        }

        void modelReset() override
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
        Model m_model;
        QAbstractItemModel* m_db_model;

        void erase(const QModelIndex& idx)
        {
            const auto key = getKey(idx);
            assert(m_model.find(key) != m_model.end());
            m_model.remove(getKey(idx));
        }

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
            return Constructor(idx);
        }
};


// A helper class which purpose is to store  in tree any model change automatically
// The only problem is that if owner of ViewDataSet (View) also connects
// itself to model's signals and will try to access ViewDataSet in
// for example rowsInserted, ViewDataSet may not be up to date yet.
// In such case, do not use this class but call all ViewDataSet's functions
// related to model changes manually.

struct ViewDataModelObserver: public QObject
{
    Q_OBJECT

    public:
        explicit ViewDataModelObserver(IViewDataSet* viewDataSet, QAbstractItemModel* model, QObject* p = 0): QObject(p), m_db_model(nullptr), m_viewDataSet(viewDataSet)
        {
            set(model);
        }

        ViewDataModelObserver(const ViewDataModelObserver &) = delete;

        ViewDataModelObserver& operator=(const ViewDataModelObserver &) = delete;

        void set(QAbstractItemModel* model)
        {
            if (m_db_model != nullptr)
                m_db_model->disconnect(this);

            m_db_model = model;

            if (m_db_model != nullptr)
            {
                connect(m_db_model, &QAbstractItemModel::rowsInserted, this, &ViewDataModelObserver::rowsInserted);
                connect(m_db_model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ViewDataModelObserver::rowsAboutToBeRemoved);
                connect(m_db_model, &QAbstractItemModel::modelReset, this, &ViewDataModelObserver::modelReset);
            }

            modelReset();
        }

    private:
        virtual void rowsInserted(const QModelIndex& p, int f, int t)         { m_viewDataSet->rowsInserted(p, f, t); }
        virtual void rowsAboutToBeRemoved(const QModelIndex& p, int f, int t) { m_viewDataSet->rowsAboutToBeRemoved(p, f, t); }
        virtual void modelReset()                                             { m_viewDataSet->modelReset(); }

    private:
        QAbstractItemModel* m_db_model;
        IViewDataSet* m_viewDataSet;
};


#endif // VIEW_DATA_SET_HPP
