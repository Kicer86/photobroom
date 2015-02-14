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

#include <deque>
#include <memory>
#include <stack>
#include <ostream>

#include <OpenLibrary/utils/data_ptr.hpp>

#include "implementation/tree_private.hpp"

template<typename T>
class tree final
{
    public:
        typedef tree_private::iterator<tree_utils::node<T>, T> iterator;
        typedef tree_private::iterator<const tree_utils::node<T>, T> const_iterator;
        typedef tree_utils::node<T> node;

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
            return iterator(&m_roots, m_roots.begin());
        }

        iterator end()
        {
            return iterator(&m_roots, m_roots.end());
        }

        const_iterator cbegin() const
        {
            return iterator(&m_roots, m_roots.begin());
        }

        const_iterator cend() const
        {
            return iterator(&m_roots, m_roots.end());
        }

        bool empty() const
        {
            return m_roots.empty();
        }

        iterator insert(iterator pos, const T& value)
        {
            const node n(value);
            auto it = pos.m_nodes->insert(pos.m_node, n);

            return iterator(pos.m_nodes, it);
        }

    private:
        tree_utils::nodes<T> m_roots;

        friend std::ostream& operator<<(std::ostream& st, const tree& tr)
        {
            st << tr.m_roots;

            return st;
        }
};


namespace tree_utils
{
    template<typename NT, typename T>
    class recursive_iterator final
    {
            typedef typename tree<T>::iterator iterator;

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
                const nodes<T>& ch = c->children();

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
            typedef std::pair<iterator, iterator> level_info;
            std::stack<level_info> m_iterators;

            iterator& current()
            {
                return m_iterators.top().first;
            }

            iterator& last()
            {
                return m_iterators.top().second;
            }
    };
}

#endif // TREE_HPP
