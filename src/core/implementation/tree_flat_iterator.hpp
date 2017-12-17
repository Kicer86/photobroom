/*
 * Tree container - flat_iterator
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

#ifndef TREE_FLAT_ITERATOR_HPP
#define TREE_FLAT_ITERATOR_HPP

#include <cassert>

#include "tree_iterator_base.hpp"

template<typename T> class tree;

namespace tree_private
{

    template<IteratorType iteratorType, typename T>
    class flat_iterator final: public iterator_base<iteratorType, T>
    {
            typedef iterator_base<iteratorType, T> base;
            typedef tree_private::node_pointer<T> node_pointer;

            explicit flat_iterator(const node_pointer& b): base(b)
            {

            }

        public:
            flat_iterator(): base() {}
            flat_iterator(const flat_iterator<iteratorType, T> &) = default;
            flat_iterator(const typename base::CopyT1& other): base(other) { }
            flat_iterator(const typename base::CopyT2& other): base(other) { }

            ~flat_iterator() {}

            flat_iterator& operator=(const flat_iterator& other)
            {
                base::operator=(other);

                return *this;
            }

            flat_iterator& operator++()
            {
                node_pointer& c = base::current();
                auto& node = *c;

                if (node.has_children())                          //dive
                    base::m_iterators.push(node.begin());
                else                                              //iterate
                {
                    bool do_jumpout = true;

                    do
                    {
                        node_pointer& cur = base::current();
                        ++cur;

                        //last one at current level? pop out an keep going
                        //anything to pop? (don't pop out from last)
                        do_jumpout = cur == base::nodes_end() &&
                                     base::m_iterators.size() > 1;

                        if (do_jumpout)
                            jumpout();
                    }
                    while(do_jumpout);
                }

                return *this;
            }

            flat_iterator operator++(int)
            {
                node_pointer it = *this;
                ++(*this);

                return it;
            }

            flat_iterator& operator--()
            {
                //step back

                //first one? go up
                node_pointer& c = base::current();
                if (c == base::nodes_begin())           //first one at current level? pop out an keep going
                {
                    if (base::m_iterators.size() > 1)   //anything to pop? (don't pop out from last)
                        jumpout();
                }
                else
                {
                    --c;

                    //go as deep as possible
                    bool dive = true;
                    do
                    {
                        node_pointer& cur = base::current();
                        //subnodes?
                        auto& node = *cur;

                        dive = node.has_children();

                        if (dive)                                     // dive
                            base::m_iterators.push(node.end() - 1);   // ommit end(), go directly to last element
                    }
                    while(dive);
                }

                return *this;
            }

            flat_iterator operator--(int)
            {
                node_pointer it = *this;
                ++(*this);

                return it;
            }

            size_t operator-(const flat_iterator<iteratorType, T>& other) const
            {
                size_t r = 0;
                for(flat_iterator<iteratorType, T> it = *this; it != other; --it)
                    r++;

                return r;
            }

        private:
            template<typename> friend class ::tree;

            void jumpout()
            {
                assert(base::m_iterators.size() > 1);

                base::m_iterators.pop();
            }
    };

}


#endif // TREE_FLAT_ITERATOR_HPP
