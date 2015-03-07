/*
 * Tree container
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

#ifndef TREE_HPP
#define TREE_HPP

#include <cassert>

#include "implementation/tree_private.hpp"
#include "implementation/tree_recursive_iterator.hpp"
#include "implementation/tree_flat_iterator.hpp"

template<typename T>
class tree final
{
        typedef typename tree_utils::node<T>::iterator node_iterator;
        typedef typename tree_utils::node<T>::const_iterator const_node_iterator;

    public:
        typedef typename tree_private::recursive_iterator<node_iterator> iterator;
        typedef typename tree_private::recursive_iterator<const_node_iterator> const_iterator;
        typedef typename tree_private::flat_iterator<node_iterator> flat_iterator;
        typedef typename tree_private::flat_iterator<const_node_iterator> const_flat_iterator;

        typedef tree_utils::node<T> node;

        tree(): m_roots()
        {
        }

        tree(const tree& other): m_roots(other.m_root)
        {
        }

        ~tree()
        {
        }

        tree& operator=(const tree& other)
        {
            if (this != &other)
                m_roots = other.m_roots;

            return *this;
        }

        bool operator==(const tree& other) const
        {
            return m_roots == other.m_roots;
        }

        //recursive iterator
        iterator begin()
        {
            node_iterator ni(&m_roots, m_roots.begin());
            return iterator(ni);
        }

        iterator end()
        {
            node_iterator ni(&m_roots, m_roots.end());
            return iterator(ni);
        }

        const_iterator begin() const
        {
            const_node_iterator ni(&m_roots, m_roots.begin());
            return const_iterator(ni);
        }

        const_iterator end() const
        {
            const_node_iterator ni(&m_roots, m_roots.end());
            return const_iterator(ni);
        }

        const_iterator cbegin() const
        {
            return begin();
        }

        const_iterator cend() const
        {
            return end();
        }

        bool empty() const
        {
            return m_roots.empty();
        }

        void clear()
        {
            m_roots.clear();
        }

        iterator insert(iterator pos, const T& value)
        {
            tree_utils::node<T> v(value);                     //prepare value
            auto l = pos.current().get_nodes_list();          //get proper sublist from iterator
            auto& it = pos.current().get_node();              //get position in list
            auto r = l->insert(it, v);

            //update iterator
            pos.current() = node_iterator(l, r);

            return pos;
        }
        
        iterator erase(iterator pos)
        {
            auto l = pos.current().get_nodes_list();          //get proper sublist from iterator
            auto& it = pos.current().get_node();              //get position in list
            auto r = l->erase(it);

            //update iterator
            pos.current() = node_iterator(l, r);

            return pos;
        }

    private:
        tree_utils::nodes<T> m_roots;

        friend std::ostream& operator<<(std::ostream& st, const tree& tr)
        {
            st << tr.m_roots;

            return st;
        }
};


#endif // TREE_HPP
