/*
 * Tree container - level_iterator
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

#ifndef TREE_LEVEL_ITERATOR_HPP
#define TREE_LEVEL_ITERATOR_HPP


#include "tree_iterator_base.hpp"

template<typename T> class tree;


namespace tree_private
{

    template<typename iterator>
    class level_iterator final: public iterator_base<iterator>
    {
            typedef iterator_base<iterator> base;

            level_iterator(const iterator& b): base(b)
            {
            }

        public:
            level_iterator(const base& other): base(other) { }

            ~level_iterator() {}

            level_iterator& operator=(const level_iterator& other)
            {
                base::operator=(other);

                return *this;
            }

            level_iterator& operator++()
            {
                iterator& c = base::current();
                ++c;

                return *this;
            }

            level_iterator operator++(int)
            {
                level_iterator it = *this;
                ++(*this);

                return it;
            }

            level_iterator& operator--()
            {
                iterator& c = base::current();
                --c;

                return *this;
            }

            level_iterator operator--(int)
            {
                iterator it = *this;
                ++(*this);

                return it;
            }

            size_t operator-(const level_iterator<iterator>& other) const
            {
                const size_t result = this->current() - other.current();

                return result;
            }

            level_iterator operator+(int v) const
            {
                level_iterator result = *this;
                iterator& c = result.current();
                c += v;

                return result;
            }

            level_iterator operator-(int v) const
            {
                level_iterator result = *this;
                iterator& c = result.current();
                c -= v;

                return result;
            }

            size_t index() const
            {
                const size_t result = this->current() - this->nodes_begin();
                return result;
            }


            level_iterator begin()
            {
                level_iterator result = *this;

                auto it = base::current()->begin();
                result.dive(it);

                return result;
            }


            level_iterator end()
            {
                level_iterator result = *this;

                auto it = base::current()->end();
                result.dive(it);

                return result;
            }

            
            size_t children_count() const
            {
                return base::current()->children_count();
            }


            level_iterator& dive(const iterator& it)
            {
                base::m_iterators.push(it);

                return *this;
            }

            level_iterator parent() const
            {
                level_iterator result = *this;

                if (result.m_iterators.size() == 1)
                    result = base::nodes_end();           // no parent
                else
                    result.m_iterators.pop();

                return result;
            }

            bool is_first() const
            {
                const bool result = base::current() == base::nodes_begin();

                return result;
            }

            bool is_last() const
            {
                const bool result = base::current() + 1 == base::nodes_end();

                return result;
            }

    };

}


// for std algorithms
namespace std
{
    template<typename iterator>
    struct iterator_traits<tree_private::level_iterator<iterator>>
    {
        typedef typename iterator::difference_type   difference_type;
        typedef typename iterator::value_type        value_type;
        typedef typename iterator::pointer           pointer;
        typedef typename iterator::reference         reference;
        typedef typename iterator::iterator_category iterator_category;
    };
}

#endif // TREE_LEVEL_ITERATOR_HPP
