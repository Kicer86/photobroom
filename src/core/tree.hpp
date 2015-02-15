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

class tree final
{
    public:

        class item
        {
            public:
                item();
                item(const item &) = delete;
                virtual ~item();

                item& operator=(const item &) = delete;

                void insert_after(item *);
                void insert_before(item *);
                void insert_first_child(item *);
                void insert_last_child(item *);

                item* parent();
                item* first_child();
                item* last_child();
                item* next();
                item* previous();

            private:
                item* m_prev;
                item* m_next;
                item* m_parent;
                item* m_first_child;
                item* m_last_child;
        };

        tree();
        ~tree();

        item* first();
        item* last();
        void insert_front(tree::item*);
        void insert_back(tree::item*);

    private:
        item m_root;
};

#endif // TREE_HPP
