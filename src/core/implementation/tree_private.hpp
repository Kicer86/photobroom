/*
 * Tree container - private implementation
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

#ifndef TREE_PRIVATE_HPP
#define TREE_PRIVATE_HPP

#include <deque>
#include <memory>
#include <stack>
#include <ostream>

#include <OpenLibrary/utils/data_ptr.hpp>

template<typename T> class tree;

namespace tree_utils
{
    template<typename T> class node;
    template<typename T> using nodes = std::deque<node<T>>;
}


namespace tree_private
{

    enum class IteratorType
    {
        Const,
        NonConst,
    };

    template<typename T, IteratorType>
    struct iterator_traits;

    template<typename T>
    struct iterator_traits<T, IteratorType::NonConst>
    {
        typedef tree_utils::nodes<T> nodes_list;
        typedef typename tree_utils::nodes<T>::iterator node_iterator;
        typedef tree_utils::node<T> node_value;
    };

    template<typename T>
    struct iterator_traits<T, IteratorType::Const>
    {
        typedef const tree_utils::nodes<T> nodes_list;
        typedef typename tree_utils::nodes<T>::const_iterator node_iterator;
        typedef const tree_utils::node<T> node_value;
    };


    template<IteratorType iteratorType, typename T>
    class iterator final
    {
        public:
            typedef typename iterator_traits<T, iteratorType>::nodes_list nodes_list;
            typedef typename iterator_traits<T, iteratorType>::node_iterator node_iterator;
            typedef typename iterator_traits<T, iteratorType>::node_value node_value;

            iterator(): m_nodes(nullptr), m_node() {}

            iterator(const iterator& other): m_nodes(nullptr), m_node(other.m_node) { }

            ~iterator() {}

            iterator& operator=(const iterator& other)
            {
                if (this != &other)
                {
                    m_nodes = other.m_nodes;
                    m_node = other.m_node;
                }

                return *this;
            }

            bool operator==(const iterator& other) const
            {
                return m_nodes == other.m_nodes &&
                       m_node == other.m_node;
            }

            bool operator!=(const iterator& other) const
            {
                const bool eq = *this == other;
                return !eq;
            }

            iterator& operator++()
            {
                ++m_node;

                return *this;
            }

            iterator operator++(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            int operator-(const iterator& it) const
            {
                return m_node - it.m_node;
            }

            iterator operator+(int o) const
            {
                iterator r(m_nodes, m_node + o);

                return r;
            }

            iterator operator-(int o) const
            {
                iterator r(m_nodes, m_node - o);

                return r;
            }

            node_value& operator*()
            {
                return *m_node;
            }

            node_value* operator->()
            {
                return &(*m_node);
            }

        private:
            friend class tree_utils::node<T>;
            friend class tree<T>;

            nodes_list* m_nodes;
            node_iterator m_node;

            iterator(nodes_list* ns, node_iterator n): m_nodes(ns), m_node(n) { }

            nodes_list* get_nodes_list()
            {
                return m_nodes;
            }

            node_iterator& get_node()
            {
                return m_node;
            }
    };
}


namespace tree_utils
{
    template<typename T>
    class node final
    {
        public:
            typedef tree_private::iterator<tree_private::IteratorType::NonConst, T> iterator;
            typedef tree_private::iterator<tree_private::IteratorType::Const, T> const_iterator;

            explicit node(const T& v): m_item(v), m_children() {}

            node(const node& other): m_item(other.m_item), m_children(other.m_children)
            {
            }

            ~node() {}

            node& operator=(const node& other)
            {
                if (this != &other)
                {
                    m_item = other.m_item;
                    m_children = other.m_children;
                }

                return *this;
            }

            bool operator==(const node& other) const
            {
                return m_item == other.m_item && m_children == other.m_children;
            }

            const T& operator*() const
            {
                return m_item;
            }

            T& operator*()
            {
                return m_item;
            }

            iterator begin()
            {
                return iterator(&m_children, m_children.begin());
            }

            iterator end()
            {
                return iterator(&m_children, m_children.end());
            }

            const_iterator begin() const
            {
                return const_iterator(&m_children, m_children.begin());
            }

            const_iterator end() const
            {
                return const_iterator(&m_children, m_children.end());
            }

            const_iterator cbegin() const
            {
                return const_iterator(&m_children, m_children.cbegin());
            }

            const_iterator cend() const
            {
                return const_iterator(&m_children, m_children.cend());
            }

            bool has_children() const
            {
                return m_children.empty() == false;
            }

            const nodes<T>& children() const
            {
                return m_children;
            }

            nodes<T>& children()
            {
                return m_children;
            }

        private:
            T m_item;
            nodes<T> m_children;

            friend std::ostream& operator<<(std::ostream& st, const node& n)
            {
                st << *n;

                return st;
            }
    };
}


template<typename T>
std::ostream& operator<<(std::ostream& st, const std::deque<tree_utils::node<T>>& nodes)
{
    st << "(";

    auto last = nodes.cend();

    for(auto it = nodes.cbegin(); it != last;)
    {
        const tree_utils::node<T>& n = *it;
        st << n;

        if (n.has_children())
            st << n.children();

        ++it;

        if (it != last)
            st << " ";
    }

    st << ")";

    return st;
}


#endif // TREE_PRIVATE_HPP

