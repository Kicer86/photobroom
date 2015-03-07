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
            typedef typename iterator::node_type RetType;

            iterator_base(const iterator& b): m_iterators()
            {
                m_iterators.push(b);
            }

            iterator_base(const iterator_base& other): m_iterators(other.m_iterators) { }

            ~iterator_base() {}

            iterator_base& operator=(const iterator_base& other)
            {
                if (*this != other)
                    m_iterators = other.m_iterators;

                return *this;
            }

            bool operator==(const iterator_base& other) const
            {
                return m_iterators == other.m_iterators;
            }

            bool operator!=(const iterator_base& other) const
            {
                return m_iterators != other.m_iterators;
            }

            const RetType& operator*() const
            {
                return *current();
            }

            RetType& operator*()
            {
                return *current();
            }

            const RetType& operator&() const
            {
                return *current();
            }

            const RetType* operator->() const
            {
                return &(*current());
            }

            RetType* operator->()
            {
                return &(*current());
            }

            bool valid() const
            {
                bool status = true;

                assert(m_iterators.empty() == false);
                if (m_iterators.size() == 1)
                    status = current() != last();

                return status;
            }

        protected:
            template<typename T> friend class tree;
            std::stack<iterator> m_iterators;

            iterator first() const
            {
                auto ns = current().get_nodes_list();
                iterator result(ns, ns->begin());
                return result;
            }

            const iterator& current() const
            {
                return m_iterators.top();
            }

            iterator last() const
            {
                auto ns = current().get_nodes_list();
                iterator result(ns, ns->end());
                return result;
            }

            iterator& current()
            {
                return m_iterators.top();
            }
    };

}

#endif // TREE_ITERATOR_BASE_HPP
