/*
 * Tree container - base for tree iterators
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

#ifndef TREE_ITERATOR_BASE_HPP
#define TREE_ITERATOR_BASE_HPP

#include <cassert>
#include <stack>

template<typename T> class tree;


namespace tree_private
{

    template<typename iterator>
    class iterator_base
    {
        public:
            typedef typename iterator::value_type RetType;

            iterator_base(const iterator& b): m_iterators()
            {
                m_iterators.push(b);
            }

            iterator_base(const iterator_base& other): m_iterators(other.m_iterators) { }

            ~iterator_base() {}

            iterator_base& operator=(const iterator_base& other)
            {
                m_iterators = other.m_iterators;

                return *this;
            }

            bool operator==(const iterator_base& other) const
            {
                return same(other);
            }

            bool operator!=(const iterator_base& other) const
            {
                return !same(other);
            }

            const RetType& operator*() const
            {
                assert(valid());
                return **current();
            }

            RetType& operator*()
            {
                assert(valid());
                return **current();
            }

            const RetType& operator&() const
            {
                assert(valid());
                return **current();
            }

            const RetType* operator->() const
            {
                assert(valid());
                return &(**current());
            }

            RetType* operator->()
            {
                assert(valid());
                return &(**current());
            }

            bool valid() const
            {
                const bool status = current() != nodes_end();

                return status;
            }

        protected:
            template<typename T> friend class tree;
            std::stack<iterator> m_iterators;

            iterator nodes_begin() const
            {
                auto ns = current().get_nodes_list();
                iterator result(ns, ns->begin());
                return result;
            }

            const iterator& current() const
            {
                return m_iterators.top();
            }

            iterator nodes_end() const
            {
                auto ns = current().get_nodes_list();
                iterator result(ns, ns->end());
                return result;
            }

            iterator& current()
            {
                return m_iterators.top();
            }

            bool same(const iterator_base& other) const
            {
                bool status = true;

                //same depth?
                if (m_iterators.size() != other.m_iterators.size())
                    status = false;
                else
                {
                    //check top items (most deep ones) if they point to the same list of nodes and to the same position
                    const iterator& t_i = m_iterators.top();
                    const iterator& o_i = other.m_iterators.top();

                    status = t_i.m_nodes == o_i.m_nodes &&
                             t_i.m_node  == o_i.m_node;
                }

                return status;
            }
    };

}

#endif // TREE_ITERATOR_BASE_HPP
