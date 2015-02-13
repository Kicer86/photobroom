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

#ifndef TREE_H
#define TREE_H

#include <deque>
#include <memory>
#include <stack>
#include <sstream>

#include <OpenLibrary/utils/data_ptr.hpp>

template<typename T>
class tree final
{
    public:
        class node;
        typedef std::deque<node> nodes;

        class node final
        {
            public:
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

                bool has_children() const
                {
                    return m_children->empty() == false;
                }

                const nodes& children() const
                {
                    return *m_children;
                }

                nodes& children()
                {
                    return *m_children;
                }

            private:
                T m_item;
                ol::data_ptr<nodes> m_children;

                friend std::ostream& operator<<(std::ostream& st, const node& n)
                {
                    st << *n;

                    return st;
                }
        };


        class iterator final
        {
            public:
                iterator(): m_tree(nullptr), m_nodes(nullptr), m_node() {}

                iterator(const iterator& other): m_tree(other.m_tree), m_nodes(nullptr), m_node(other.m_node) { }

                ~iterator() {}

                iterator& operator=(const iterator& other)
                {
                    if (this != &other)
                    {
                        m_tree = other.m_tree;
                        m_nodes = other.m_nodes;
                        m_node = other.m_node;
                    }

                    return *this;
                }

                bool operator==(const iterator& other) const
                {
                    return m_tree == other.m_tree &&
                           m_nodes == other.m_nodes &&
                           m_node == other.m_node;
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

                T& operator*()
                {
                    return *m_node;
                }

                const T& operator&() const
                {
                    return *m_node;
                }

                T* operator->()
                {
                    return *m_node;
                }

                const T* operator->() const
                {
                    return *m_node;
                }

                iterator children_begin() const
                {
                    return iterator(m_tree, &m_node->children(), m_node->children().begin());
                }

                iterator children_end() const
                {
                    return iterator(m_tree, &m_node->children(), m_node->children().end());
                }

                bool has_children() const
                {
                    return m_node->has_children();
                }

                const nodes& children() const
                {
                    return m_node->children();
                }

            private:
                friend class tree;

                tree*  m_tree;
                nodes* m_nodes;
                typename nodes::iterator m_node;

                iterator(tree* tr, nodes* ns, typename nodes::iterator n): m_tree(tr), m_nodes(ns), m_node(n) { }
        };


        class recursive_iterator final
        {
            public:
                recursive_iterator(const iterator& b, const iterator& e): m_iterators()
                {
                    m_iterators.push(std::make_pair(b, e));
                }

                recursive_iterator(const recursive_iterator& other): m_iterators(other.iterators) { }

                ~recursive_iterator() {}

                recursive_iterator& operator=(const recursive_iterator& other)
                {
                    if (this != &other)
                        m_iterators = other.m_iterators;

                    return *this;
                }

                bool operator==(const recursive_iterator& other) const
                {
                    return m_iterators == other.m_iterators;
                }

                recursive_iterator& operator++()
                {
                    iterator& c = current();
                    const nodes& ch = c->children();

                    if (ch.empty() == false)                          //dive
                        m_iterators.push(ch.begin(), ch.end());
                    else
                    {                                                 //iterate
                        ++c;

                        if (c == last())                              //last one at current level? pop out an keep going
                        {
                            if (m_iterators.size() > 1)               //anything to pop? (don't pop out from last)
                            {
                                m_iterators.pop();
                                ++(*this);
                            }
                        }
                    }

                    return *this;
                }

                recursive_iterator operator++(int)
                {
                    iterator it = *this;
                    ++(*this);

                    return it;
                }

                T& operator*()
                {
                    return *current();
                }

                const T& operator&() const
                {
                    return *current();
                }

                T* operator->()
                {
                    return *current();
                }

                const T* operator->() const
                {
                    return *current();
                }

            private:
                std::stack< std::pair<iterator, iterator> > m_iterators;

                iterator& current()
                {
                    return m_iterators.top().first;
                }

                iterator& last()
                {
                    return m_iterators.top().second;
                }

        };


        tree(): m_roots()
        {
        }

        tree(const tree& other): m_roots(other.m_roots)
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

        iterator begin()
        {
            return iterator(this, &m_roots, m_roots.begin());
        }

        iterator end()
        {
            return iterator(this, &m_roots, m_roots.end());
        }

        bool empty() const
        {
            return m_roots.empty();
        }

        iterator insert(iterator pos, const T& value)
        {
            const node n(value);
            auto it = pos.m_nodes->insert(pos.m_node, n);

            return iterator(this, pos.m_nodes, it);
        }

        std::string dump() const
        {
            std::stringstream d;

            d << m_roots;

            return d.str();
        }

    private:
        nodes m_roots;

        friend std::ostream& operator<<(std::ostream& st, const nodes& ns)
        {
            st << "(";

            for(auto it = ns.begin(); it != ns.end();)
            {
                const node& n = *it;
                st << n;

                if (n.has_children())
                    st << n.children();

                ++it;

                if (it != ns.end())
                    st << " ";
            }

            st << ")";

            return st;
        }
};

#endif // TREE_H
