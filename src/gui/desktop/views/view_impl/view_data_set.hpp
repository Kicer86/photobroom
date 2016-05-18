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
#include <functional>
#include <iostream>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QRect>

#ifndef NDEBUG
#define assert_dump(expr,dump)                          \
    if (!expr) { dump(), abort(); }                     \
    else static_cast<void>(0)
#else
#define assert_dump(expr,dump) static_cast<void>(0)
#endif


void for_each_child(QAbstractItemModel *, const QModelIndex &, std::function<void(const QModelIndex &)>);
void for_each_child_deep(QAbstractItemModel *, const QModelIndex &, std::function<void(const QModelIndex &)>);


struct IViewDataSet
{
        virtual ~IViewDataSet();

        virtual void rowsInserted(const QModelIndex &, int, int) = 0;
        virtual void rowsRemoved(const QModelIndex &, int, int) = 0;
        virtual void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int) = 0;
        virtual void modelReset() = 0;
};


template<typename T>
class ViewDataSet final: public IViewDataSet
{
        template<typename IT, typename M>
        IT _find(M& model, const std::vector<size_t>& hierarchy) const
        {
            assert(hierarchy.empty() == false);

            //setup first item
            IT item_it = model.end();

            if (model.empty() == false)
                for(size_t i = 0; i < hierarchy.size(); i++)
                {
                    const size_t pos = hierarchy[i];

                    if (i == 0)
                    {
                        IT b(model.begin());
                        IT e(model.end());

                        const size_t c = e - b;               //how many items?
                        if (pos < c)
                            item_it = IT(b) + pos;
                        else
                        {
                            item_it = e;
                            break;                            //out of scope
                        }
                    }
                    else
                    {
                        const size_t c = item_it.children_count();

                        if (pos < c)
                            item_it = item_it.begin() + pos;
                        else
                        {
                            item_it = model.end();
                            break;                            //out of scope
                        }
                    }
                }

            return item_it;
        }

    public:
        typedef std::map<quintptr, T>  Model;
        typedef typename Model::const_iterator const_iterator;
        typedef typename Model::iterator       iterator;

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

        const_iterator find(const QModelIndex& index) const
        {
            return m_model.find(index.internalId());
        }

        const_iterator begin() const
        {
            return m_model.begin();
        }

        const_iterator end() const
        {
            return m_model.end();
        }

        const_iterator cfind(const QModelIndex& index) const
        {
            return find(index);
        }

        const_iterator cbegin() const
        {
            return m_model.cbegin();
        }

        const_iterator cend() const
        {
            return m_model.cend();
        }

        iterator find(const QModelIndex& index)
        {
            return m_model.find(index.internalId());
        }

        iterator begin()
        {
            return m_model.begin();
        }

        iterator end()
        {
            return m_model.end();
        }

        bool empty() const
        {
            return m_model.empty();
        }

        size_t size() const
        {
            return m_model.size();
        }

        std::string dumpModel() const
        {
            const std::string dump = m_model.dump();
            return dump;
        }

        // to be called by view:
        void rowsInserted(const QModelIndex& parent, int from, int to) override
        {
            //update model
            for( int i = from; i <= to; i++)
            {
                const QModelIndex child_idx = m_db_model->index(i, 0, parent);
                insert(child_idx, T(child_idx));

                //check if inserted item has children, and add them if any
                const int children = m_db_model->rowCount(child_idx);
                if (children > 0)
                    rowsInserted(child_idx, 0, children - 1);
            }
        }

        void rowsRemoved(const QModelIndex& parent, int from , int to) override
        {
            //update model

                for(int i = from; i <= to; i++)
                {
                    const QModelIndex child = parent.child(i, 0);
                    m_model.erase(child.internalId());
                }

        }

        void rowsMoved(const QModelIndex& sourceParent, int src_from, int src_to, const QModelIndex& destinationParent, int dst_from) override
        {
            const int n = src_to - src_from;
            const int dst_to = dst_from + n;

            // TODO: implement variant which would do a real move

            if (sourceParent != destinationParent || src_from > dst_to)
            {
                rowsRemoved(sourceParent, src_from, src_to);
                rowsInserted(destinationParent, dst_from, dst_to);
            }
            else
            {
                // The same parent, and source rows are before destination rows.
                // In such case we need to do some adjustments in destination row

                const int rows_removed = n + 1;

                rowsRemoved(sourceParent, src_from, src_to);
                rowsInserted(destinationParent, dst_from - rows_removed , dst_to - rows_removed);
            }
        }

        void modelReset() override
        {
            clear();

            for_each_child_deep(m_db_model, QModelIndex(), [&](const QModelIndex& idx)
            {
                insert(idx, T(idx));
            });
        }

    private:
        Model m_model;
        QAbstractItemModel* m_db_model;

        void erase(const iterator& it)
        {
            m_model.erase(it);
        }

        void clear()
        {
            m_model.clear();

            //add item for QModelIndex() which is always present
            insert( QModelIndex(), T(QModelIndex()) );
        }

        iterator insert(const QModelIndex& index, const T& info)
        {
            const quintptr id = index.internalId();
            auto it = m_model.insert( std::make_pair(id, info) );

            return it.first;
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
                connect(m_db_model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(rowsInserted(QModelIndex, int, int)));
                connect(m_db_model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(rowsRemoved(QModelIndex, int, int)));
                connect(m_db_model, SIGNAL(rowsMoved(QModelIndex,int, int, QModelIndex, int)),
                        this, SLOT(rowsMoved(QModelIndex,int, int, QModelIndex, int)));

                connect(m_db_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
            }

            modelReset();
        }

    private slots:
        virtual void rowsInserted(const QModelIndex& p, int f, int t) { m_viewDataSet->rowsInserted(p, f, t); }
        virtual void rowsRemoved(const QModelIndex& p, int f, int t)  { m_viewDataSet->rowsRemoved(p, f, t); }
        virtual void rowsMoved(const QModelIndex& p, int f, int t,
                               const QModelIndex& d, int dt)          { m_viewDataSet->rowsMoved(p, f, t, d, dt); }
        virtual void modelReset()                                     { m_viewDataSet->modelReset(); }

    private:
        QAbstractItemModel* m_db_model;
        IViewDataSet* m_viewDataSet;
};


#endif // VIEW_DATA_SET_HPP
