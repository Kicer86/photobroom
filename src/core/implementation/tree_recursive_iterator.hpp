/*
 * Tree container - recursive_iterator
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

#ifndef TREE_RECURSIVE_ITERATOR_HPP
#define TREE_RECURSIVE_ITERATOR_HPP

#include <stack>

#include <OpenLibrary/utils/data_ptr.hpp>

template<typename T> class tree;


namespace tree_private
{

    template<typename iterator>
    class recursive_iterator final
    {
        public:
            typedef typename iterator::node_type RetType;

            recursive_iterator(const iterator& b): m_iterators()
            {
                m_iterators.push(b);
            }

            recursive_iterator(const recursive_iterator& other): m_iterators(other.m_iterators) { }

            ~recursive_iterator() {}

            recursive_iterator& operator=(const recursive_iterator& other)
            {
                if (*this != other)
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
                    m_iterators.push(node.begin());
                else                                              //iterate
                {
                    bool do_jumpout = true;

                    do
                    {
                        iterator& cur = current();
                        ++cur;

                        //last one at current level? pop out an keep going
                        //anything to pop? (don't pop out from last)
                        do_jumpout = cur == last() && m_iterators.size() > 1;

                        if (do_jumpout)
                            jumpout();
                    }
                    while(do_jumpout);
                }

                return *this;
            }

            recursive_iterator operator++(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            recursive_iterator& operator--()
            {
                //step back

                //first one? go up
                iterator& c = current();
                if (c == first())                             //first one at current level? pop out an keep going
                {
                    if (m_iterators.size() > 1)               //anything to pop? (don't pop out from last)
                        jumpout();
                }
                else
                {
                    --c;

                    //go as deep as possible
                    bool dive = true;
                    do
                    {
                        iterator& cur = current();
                        //subnodes?
                        auto& node = *cur;

                        dive = node.has_children();

                        if (dive)                                     // dive
                            m_iterators.push(node.end() - 1);         // ommit end(), go directly to last element
                    }
                    while(dive);
                }

                return *this;
            }

            recursive_iterator operator--(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
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

            operator iterator()
            {
                return current();
            }

            operator iterator() const
            {
                return current();
            }

            size_t operator-(const recursive_iterator<iterator>& other) const
            {
                size_t r = 0;
                for(recursive_iterator<iterator> it = *this; it != other; --it)
                    r++;

                return r;
            }

            bool valid() const
            {
                bool status = true;

                assert(m_iterators.empty() == false);
                if (m_iterators.size() == 1)
                    status = current() != last();

                return status;
            }

            recursive_iterator parent() const
            {
                assert(m_iterators.empty() == false);

                recursive_iterator result = last();

                if (m_iterators.size() > 1)
                {
                    result = *this;
                    result.jumpout();
                }

                return result;
            }

        private:
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

            void jumpout()
            {
                assert(m_iterators.size() > 1);

                m_iterators.pop();
            }
    };

}



#endif // TREE_RECURSIVE_ITERATOR_HPP

