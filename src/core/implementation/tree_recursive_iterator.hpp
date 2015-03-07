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

#include <cassert>

#include "tree_iterator_base.hpp"


namespace tree_private
{

    template<typename iterator>
    class recursive_iterator final: public iterator_base<iterator>
    {
            typedef iterator_base<iterator> base;

        public:
            recursive_iterator(const iterator& b): base(b)
            {
            }

            recursive_iterator(const base& other): base(other) { }

            ~recursive_iterator() {}

            recursive_iterator& operator=(const recursive_iterator& other)
            {
                base::operator=(other);

                return *this;
            }

            recursive_iterator& operator++()
            {
                iterator& c = base::current();
                auto& node = *c;

                if (node.has_children())                          //dive
                    base::m_iterators.push(node.begin());
                else                                              //iterate
                {
                    bool do_jumpout = true;

                    do
                    {
                        iterator& cur = base::current();
                        ++cur;

                        //last one at current level? pop out an keep going
                        //anything to pop? (don't pop out from last)
                        do_jumpout = cur == base::last() &&
                                     base::m_iterators.size() > 1;

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
                iterator& c = base::current();
                if (c == base::first())                 //first one at current level? pop out an keep going
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
                        iterator& cur = base::current();
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

            recursive_iterator operator--(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            size_t operator-(const recursive_iterator<iterator>& other) const
            {
                size_t r = 0;
                for(recursive_iterator<iterator> it = *this; it != other; --it)
                    r++;

                return r;
            }

            recursive_iterator parent() const
            {
                assert(base::m_iterators.empty() == false);

                recursive_iterator result = base::last();

                if (base::m_iterators.size() > 1)
                {
                    result = *this;
                    result.jumpout();
                }

                return result;
            }

        private:
            void jumpout()
            {
                assert(base::m_iterators.size() > 1);

                base::m_iterators.pop();
            }
    };

}



#endif // TREE_RECURSIVE_ITERATOR_HPP

