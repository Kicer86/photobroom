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
                node(): m_children() {}

                node(const node& other): m_item(other.m_item), m_children(other.m_children)
                {
                }

                ~node() {}

                node& operator=(const node& other)
                {
                    m_item = other.m_item;
                    m_children = other.m_children;

                    return *this;
                }

                bool operator==(const node& other) const
                {
                    return m_item == other.m_item && m_children == other.m_children;
                }

            private:
                T m_item;

                ol::data_ptr<nodes> m_children;
        };


        class iterator final
        {
            public:
                iterator(): m_tree(nullptr), m_node(nullptr) {}

                iterator(const iterator& other)
                {
                    m_tree = other.m_tree;
                    m_node = other.m_node;
                }

                ~iterator() {}

                iterator& operator=(const iterator& other)
                {
                    m_tree = other.m_tree;
                    m_node = other.m_node;

                    return *this;
                }

                bool operator==(const iterator& other) const
                {
                    return m_tree == other.m_tree && m_node == other.node;
                }

            private:
                tree* m_tree;
                node* m_node;
        };


        tree()
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
            m_roots = other.m_roots;

            return *this;
        }

        bool operator==(const tree& other) const
        {
            return m_roots == other.m_roots;
        }

    private:
        nodes m_roots;
};

#endif // TREE_H
