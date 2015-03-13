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


#include "tree_iterator_base.hpp"

template<typename T> class tree;


namespace tree_private
{

    template<typename iterator>
    class flat_iterator final: public iterator_base<iterator>
    {
            typedef iterator_base<iterator> base;

            flat_iterator(const iterator& b): base(b)
            {
            }

        public:
            flat_iterator(const base& other): base(other) { }

            ~flat_iterator() {}

            flat_iterator& operator=(const flat_iterator& other)
            {
                base::operator=(other);

                return *this;
            }

            flat_iterator& operator++()
            {
                iterator& c = base::current();
                ++c;

                return *this;
            }

            flat_iterator operator++(int)
            {
                flat_iterator it = *this;
                ++(*this);

                return it;
            }

            flat_iterator& operator--()
            {
                iterator& c = base::current();
                --c;

                return *this;
            }

            flat_iterator operator--(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            size_t operator-(const flat_iterator<iterator>& other) const
            {
                const size_t result = this->current() - other.current();

                return result;
            }

            flat_iterator operator+(int v) const
            {
                flat_iterator result = *this;
                iterator& c = result.current();
                c += v;

                return result;
            }

            size_t index() const
            {
                const size_t result = this->current() - this->first();
                return result;
            }


            flat_iterator begin()
            {
                flat_iterator result = *this;

                auto it = base::current()->begin();
                result.dive(it);

                return result;
            }


            flat_iterator end()
            {
                flat_iterator result = *this;

                auto it = base::current()->end();
                result.dive(it);

                return result;
            }

            
            size_t children_count() const
            {
                return base::current()->children_count();
            }


            flat_iterator& dive(const iterator& it)
            {
                base::m_iterators.push(it);

                return *this;
            }

            flat_iterator parent() const
            {
                flat_iterator result = *this;

                if (result.m_iterators.size() == 1)
                    result = base::last();           // no parent
                else
                    result.m_iterators.pop();

                return result;
            }

    };

}



#endif // TREE_RECURSIVE_ITERATOR_HPP

