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

#include <cassert>

#include "tree.hpp"


tree::item::item(): m_prev(nullptr), m_next(nullptr), m_parent(nullptr), m_first_child(nullptr), m_last_child(nullptr)
{

}


tree::item::~item()
{
    //delete children
    item* c = first_child();

    while(c != nullptr)
    {
        item* del = c;
        c = c->next();

        delete del;
    }

    //detach from siblings
    if (m_prev != nullptr)
        m_prev->m_next = m_next;

    if (m_next != nullptr)
        m_next->m_prev = m_prev;

    //detach from parent
    if (m_parent)
    {
        if (m_parent->m_first_child == this)
            m_parent->m_first_child = m_next;

        if (m_parent->m_last_child == this)
            m_parent->m_last_child = m_prev;
    }
}


void tree::item::insert_after(tree::item* i)
{
    //setup new item
    assert(i->m_prev == nullptr);
    assert(i->m_next == nullptr);
    assert(i->m_parent == nullptr);
    i->m_parent = m_parent;
    i->m_prev = this;
    i->m_next = m_next;

    //setup next item
    if (m_next != nullptr)
        m_next->m_prev = i;

    //setup this
    m_next = i;

    //setup parent
    if (m_parent != nullptr && i->m_next == nullptr)  //'i' is last?
        m_parent->m_last_child = i;
}


void tree::item::insert_before(tree::item* i)
{
    //setup new item
    assert(i->m_prev == nullptr);
    assert(i->m_next == nullptr);
    assert(i->m_parent == nullptr);
    i->m_parent = m_parent;
    i->m_prev = m_prev;
    i->m_next = this;

    //setup previous item
    if (m_prev != nullptr)
        m_prev->m_next = i;

    //setup this
    m_prev = i;

    //setup parent
    if (m_parent != nullptr && i->m_prev == nullptr)  //'i' is first?
        m_parent->m_first_child = i;
}


void tree::item::insert_first_child(item* i)
{
    //setup currently first
    if (m_first_child != nullptr)
        m_first_child->m_prev = i;

    //setup new
    assert(i->m_prev == nullptr);
    assert(i->m_next == nullptr);
    assert(i->m_parent == nullptr);
    i->m_next = m_first_child;
    i->m_prev = nullptr;
    i->m_parent = this;

    //setup this
    m_first_child = i;

    if (m_last_child == nullptr)
        m_last_child = i;
}


void tree::item::insert_last_child(item* i)
{
    //setup currently last
    if (m_last_child != nullptr)
        m_last_child->m_next = i;

    //setup new
    assert(i->m_prev == nullptr);
    assert(i->m_next == nullptr);
    assert(i->m_parent == nullptr);
    i->m_next = nullptr;
    i->m_prev = m_last_child;
    i->m_parent = this;

    //setup this
    m_last_child = i;

    if (m_first_child == nullptr)
        m_first_child = i;
}


tree::item* tree::item::parent()
{
    return m_parent;
}


tree::item* tree::item::first_child()
{
    return m_first_child;
}


tree::item* tree::item::last_child()
{
    return m_last_child;
}


tree::item* tree::item::next()
{
    return m_next;
}


tree::item* tree::item::previous()
{
    return m_prev;
}


///////////////////////////////////////////////////////////////////////////////


tree::tree(): m_root()
{

}


tree::~tree()
{

}


tree::item* tree::first()
{
    return m_root.first_child();
}


tree::item* tree::last()
{
    return m_root.last_child();
}


void tree::insert_front(item* i)
{
    m_root.insert_first_child(i);
}


void tree::insert_back(item* i)
{
    m_root.insert_last_child(i);
}
