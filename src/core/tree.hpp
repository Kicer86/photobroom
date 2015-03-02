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

template<typename T>
class tree final
{
    public:
        typedef typename tree_utils::node<T>::iterator iterator;
        typedef typename tree_utils::node<T>::const_iterator const_iterator;

        typedef tree_utils::node<T> node;

        tree(): m_root(T())
        {
        }

        tree(const tree& other): m_root(other.m_root)
        {
        }

        ~tree()
        {
        }

        tree& operator=(const tree& other)
        {
            if (this != &other)
                m_root = other.m_root;

            return *this;
        }

        bool operator==(const tree& other) const
        {
            return m_root == other.m_root;
        }

        iterator begin()
        {
            return m_root.begin();
        }

        iterator end()
        {
            return m_root.end();
        }

        const_iterator begin() const
        {
            return m_root.begin();
        }

        const_iterator end() const
        {
            return m_root.end();
        }

        const_iterator cbegin() const
        {
            return m_root.cbegin();
        }

        const_iterator cend() const
        {
            return m_root.cend();
        }

        bool empty() const
        {
            return m_root.has_children() == false;
        }

        void clear()
        {
            m_root.clear();
        }

        iterator insert(iterator pos, const T& value)
        {
            tree_utils::node<T> v(value);           //prepare value
            auto l = pos.get_nodes_list();          //get proper sublist from iterator
            auto& it = pos.get_node();              //get position in list
            auto r = l->insert(it, v);

            return iterator(l, r);
        }
        
        iterator erase(iterator pos)
        {
            auto l = pos.get_nodes_list();          //get proper sublist from iterator
            auto& it = pos.get_node();              //get position in list            
            auto r = l->erase(it);

            return iterator(l, r);
        }

    private:
        tree_utils::node<T> m_root;

        friend std::ostream& operator<<(std::ostream& st, const tree& tr)
        {
            st << tr.m_root.children();

            return st;
        }
};


namespace tree_utils
{
    template<typename iterator>
    class recursive_iterator final
    {
        public:
            typedef typename iterator::value_type RetType;

            recursive_iterator(const iterator& b, const iterator& e): m_iterators()
            {
                m_iterators.push(std::make_pair(b, e));
            }

            recursive_iterator(const recursive_iterator& other): m_iterators(other.m_iterators) { }

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

            bool operator!=(const recursive_iterator& other) const
            {
                return m_iterators != other.m_iterators;
            }

            recursive_iterator& operator++()
            {
                iterator& c = current();
                auto& node = *c;

                if (node.has_children())                          //dive
                {
                    level_info level(node.begin(), node.end());
                    m_iterators.push(level);
                }
                else                                              //iterate
                    increment_current();

                return *this;
            }

            recursive_iterator operator++(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            const RetType& operator*() const
            {
                return **current();
            }

            RetType& operator*()
            {
                return **current();
            }

            const RetType& operator&() const
            {
                return *current();
            }

            const RetType* operator->() const
            {
                return *current();
            }

            RetType* operator->()
            {
                return *current();
            }
            
            operator iterator()
            {
                return current();
            }
            
            operator iterator() const
            {
                return current();
            }

            bool valid() const
            {
                bool status = true;

                assert(m_iterators.empty() == false);
                if (m_iterators.size() == 1)
                    status = current() != last();

                return status;
            }

        private:
            typedef std::pair<iterator, iterator> level_info;
            std::stack<level_info> m_iterators;

            const iterator& current() const
            {
                return m_iterators.top().first;
            }

            const iterator& last() const
            {
                return m_iterators.top().second;
            }

            iterator& current()
            {
                return m_iterators.top().first;
            }

            iterator& last()
            {
                return m_iterators.top().second;
            }

            void increment_current()
            {
                iterator& c = current();
                ++c;

                if (c == last())                              //last one at current level? pop out an keep going
                {
                    if (m_iterators.size() > 1)               //anything to pop? (don't pop out from last)
                    {
                        m_iterators.pop();

                        increment_current();
                    }
                }
            }
    };


    //helper functions
    template<typename T>
    recursive_iterator<T> make_recursive_iterator(const T& b, const T& e)
    {
        return recursive_iterator<T>(b, e);
    }

    template<typename T>
    auto make_recursive_iterator(const tree<T>& t) -> decltype( make_recursive_iterator(t.begin(), t.end()) )
    {
        return make_recursive_iterator(t.begin(), t.end());
    }

    template<typename T>
    auto make_recursive_iterator(tree<T>& t) -> decltype( make_recursive_iterator(t.begin(), t.end()) )
    {
        return make_recursive_iterator(t.begin(), t.end());
    }
}

#endif // TREE_HPP
