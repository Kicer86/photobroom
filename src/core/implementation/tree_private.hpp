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

#include <cassert>
#include <vector>
#include <memory>
#include <ostream>
#include <stack>

#include <OpenLibrary/utils/data_ptr.hpp>

template<typename T> class tree;

namespace tree_utils
{
    template<typename T> class node;
    template<typename T> using nodes = std::vector<node<T>>;
}


namespace tree_private
{

    enum class IteratorType
    {
        Const,
        NonConst,
    };

    template<tree_private::IteratorType, typename T> class iterator_base;

    template<typename T>
    class node_pointer final
    {
        public:
            typedef typename tree_utils::nodes<T>           nodes_list;
            typedef typename tree_utils::nodes<T>::iterator node_iterator;
            typedef typename tree_utils::node<T>            node_type;

            node_pointer(): m_nodes(nullptr), m_node() {}

            node_pointer(const node_pointer& other): m_nodes(other.m_nodes), m_node(other.m_node) { }

            ~node_pointer() {}

            node_pointer& operator=(const node_pointer& other)
            {
                if (this != &other)
                {
                    m_nodes = other.m_nodes;
                    m_node = other.m_node;
                }

                return *this;
            }

            bool operator==(const node_pointer& other) const
            {
                return m_nodes == other.m_nodes &&
                       m_node == other.m_node;
            }

            bool operator!=(const node_pointer& other) const
            {
                const bool eq = *this == other;
                return !eq;
            }

            node_pointer& operator++()
            {
                ++m_node;

                return *this;
            }

            node_pointer operator++(int)
            {
                node_pointer it = *this;
                ++(*this);

                return it;
            }

            node_pointer& operator--()
            {
                --m_node;

                return *this;
            }

            int operator-(const node_pointer& it) const
            {
                return m_node - it.m_node;
            }

            node_pointer operator+(int o) const
            {
#ifndef NDEBUG
                const size_t cur_pos = m_node - m_nodes->begin();
                const size_t container_size = m_nodes->size();
                assert(container_size >= cur_pos + o);               // is there enought elements?
#endif
                node_pointer r(m_nodes, m_node + o);

                return r;
            }

            node_pointer operator+=(int o)
            {
#ifndef NDEBUG
                const size_t cur_pos = m_node - m_nodes->begin();
                const size_t container_size = m_nodes->size();
                assert(container_size >= cur_pos + o);               // is there enought elements?
#endif
                m_node += o;

                return *this;
            }

            node_pointer operator-(int o) const
            {
#ifndef NDEBUG
                const int cur_pos = m_node - m_nodes->begin();
                assert(cur_pos >= o);
#endif
                node_pointer r(m_nodes, m_node - o);

                return r;
            }

            node_pointer operator-=(int v)
            {
#ifndef NDEBUG
                const int cur_pos = m_node - m_nodes->begin();
                assert(cur_pos >= v);
#endif
                m_node -= v;

                return *this;
            }

            const node_type& operator*() const
            {
                return *m_node;
            }

            node_type& operator*()
            {
                return *m_node;
            }

            const node_type* operator->() const
            {
                return &(*m_node);
            }

            node_type* operator->()
            {
                return &(*m_node);
            }

        private:
            friend class tree_utils::node<T>;
            friend class tree<T>;
            template<IteratorType, typename> friend class iterator_base;

            nodes_list* m_nodes;
            node_iterator m_node;

            node_pointer(nodes_list* ns, node_iterator n): m_nodes(ns), m_node(n) { }

            nodes_list* get_nodes_list() const
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
            typedef tree_private::node_pointer<T> iterator;

            explicit node(const T& v): m_item(v), m_children(new nodes<T>) {}

            node(const node& other): m_item(other.m_item), m_children(new nodes<T>(*other.m_children))
            {
            }

            ~node()
            {
                delete m_children;
            }

            node& operator=(const node& other)
            {
                if (this != &other)
                {
                    m_item = other.m_item;
                    *m_children = *other.m_children;
                }

                return *this;
            }

            node& operator=(const T& v)
            {
                m_item = v;

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

            operator T&()
            {
                return m_item;
            }

            operator const T&() const
            {
                return m_item;
            }

            iterator begin() const
            {
                return iterator(m_children, m_children->begin());
            }

            iterator end() const
            {
                return iterator(m_children, m_children->end());
            }

            bool has_children() const
            {
                return m_children->empty() == false;
            }

            size_t children_count() const
            {
                return m_children->size();
            }

            const nodes<T>& children() const
            {
                return *m_children;
            }

            nodes<T>& children()
            {
                return *m_children;
            }

            void clear()
            {
                m_children->clear();
            }

        private:
            T m_item;
            nodes<T>* m_children;

            friend std::ostream& operator<<(std::ostream& st, const node& n)
            {
                st << *n;

                return st;
            }
    };
}


template<typename T>
std::ostream& operator<<(std::ostream& st, const std::vector<tree_utils::node<T>>& nodes)
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

