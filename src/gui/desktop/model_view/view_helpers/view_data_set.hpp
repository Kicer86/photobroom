/*
 * View's extra data set
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

#include <QModelIndex>
#include <QRect>

#include <core/tree.hpp>

struct ModelObserverInterface: public QObject
{
    Q_OBJECT

    public:
        explicit ModelObserverInterface(QObject* p = 0): QObject(p), m_model(nullptr)
        {
        }

        void set(QAbstractItemModel* model)
        {
            if (m_model != nullptr)
                m_model->disconnect(this);

            m_model = model;

            if (m_model != nullptr)
            {
                connect(m_model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(rowsInserted(QModelIndex, int, int)));
                connect(m_model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(rowsRemoved(QModelIndex, int, int)));
                connect(m_model, SIGNAL(rowsMoved(QModelIndex,int, int, QModelIndex, int)),
                        this, SLOT(rowsMoved(QModelIndex,int, int, QModelIndex, int)));

                connect(m_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
            }

            modelReset();
        }

    protected slots:
        virtual void rowsInserted(const QModelIndex &, int, int) = 0;
        virtual void rowsRemoved(const QModelIndex &, int, int) = 0;
        virtual void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int) = 0;
        virtual void modelReset() = 0;

    protected:
        QAbstractItemModel* m_model;
};

template<typename T>
class ViewDataSet final: ModelObserverInterface
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
        typedef tree<T>  Model;
        typedef typename Model::const_iterator const_iterator;
        typedef typename Model::iterator       iterator;

        typedef typename Model::const_flat_iterator const_flat_iterator;
        typedef typename Model::flat_iterator       flat_iterator;

        ViewDataSet(): m_model()
        {
        }

        ~ViewDataSet()
        {
        }

        const_iterator find(const QModelIndex& index) const
        {
            std::vector<size_t> hierarchy = generateHierarchy(index);

            return _find<const_flat_iterator>(m_model, hierarchy);
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
            std::vector<size_t> hierarchy = generateHierarchy(index);

            return _find<flat_iterator>(m_model, hierarchy);
        }

        iterator begin()
        {
            return m_model.begin();
        }

        iterator end()
        {
            return m_model.end();
        }

        void clear()
        {
            m_model.clear();
        }

        void erase(const iterator& it)
        {
            m_model.erase(it);
        }

        iterator insert(const QModelIndex& index, const T& info)
        {
            assert(find(index) == end());                 // we do not expect this item already in model

            std::vector<size_t> hierarchy = generateHierarchy(index);
            const size_t hierarchy_size = hierarchy.size();
            assert(hierarchy_size > 0);

            //setup first item
            flat_iterator item_it = m_model.end();

            for(size_t i = 0; i < hierarchy.size(); i++)
            {
                const size_t pos = hierarchy[i];
                const bool last = i + 1 == hierarchy_size;

                if (i == 0)
                {
                    flat_iterator b(m_model.begin());
                    flat_iterator e(m_model.end());

                    const size_t c = e - b;               // how many items?
                    if (pos < c)
                    {
                        assert(last == false);            // we want to insert item. If this is last level and there is such item, something went wrong (assert at top of the function should have catch it)
                        item_it = flat_iterator(m_model.begin()) + hierarchy[i];
                    }
                    else if (pos == c)                    // just append after last item?
                    {
                        if (last == false)                // for last level do nothing - we will instert this item later below
                        {
                            flat_iterator ins = b + pos;
                            item_it = m_model.insert(ins, T());
                        }
                    }
                    else
                        assert(!"missing siblings");
                }
                else
                {
                    const size_t c = item_it.children_count();
                    if (pos < c)
                    {
                        assert(last == false);            // we want to insert item. If this is last level and there is such item, something went wrong (assert at top of the function should have catch it)
                        item_it = item_it.begin() + pos;
                    }
                    else if (pos == c)                    //just append after last item?
                    {
                        flat_iterator ins = item_it.begin() + pos;

                        if (last)
                            item_it = ins;                // for last level of hierarchy set item_it to desired position
                            else
                                item_it = m_model.insert(ins, T());
                    }
                    else
                        assert(!"missing siblings");
                }
            }

            auto it = m_model.insert(item_it, info);

            return it;
        }

        iterator insert(flat_iterator it, const T& info)
        {
            return m_model.insert(it, info);
        }

        bool empty() const
        {
            return m_model.empty();
        }

        size_t size() const
        {
            return m_model.cend() - m_model.cbegin();
        }
        
        std::string dumpModel() const
        {
            const std::string dump = m_model.dump();
            return dump;
        }

    private:
        Model m_model;
        
        std::vector<size_t> generateHierarchy(const QModelIndex& index) const
        {
            std::vector<size_t> result;

            if (index.isValid())
            {
                std::vector<size_t> parents = generateHierarchy(index.parent());
                result.insert(result.begin(), parents.cbegin(), parents.cend());
                result.push_back(index.row());
            }
            else
                result.push_back(0);             //top item

                return result;
        }

        void rowsInserted(const QModelIndex &, int, int) override
        {
        }

        void rowsRemoved(const QModelIndex &, int, int) override
        {
        }

        void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int) override
        {
        }

        void modelReset() override
        {
        }
};

#endif // VIEW_DATA_SET_HPP
