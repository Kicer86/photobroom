/*
 * View's data structure
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#include "data.hpp"

#include <cassert>
#include <iostream>

#include <QPixmap>
#include <QIcon>
#include <QDebug>


std::ostream &operator<<(std::ostream &o, const QRect &r)
{
    return o << "[left: " << r.left () << "; top: " << r.top() << "; right: " << r.right() << "; bottom: " << r.bottom() << "]";
}


std::ostream& operator<<( std::ostream& os, const QModelIndex& idx )
{
    QString str;
    QDebug(&str) << idx;

    os << str.toStdString();

    return os;
}


//////////////////////////////////////////////////////////////////////////////////////


Data::~Data()
{

}


void Data::set(QAbstractItemModel* model)
{
    m_model = model;
}


ModelIndexInfoSet::iterator Data::get(const QModelIndex& index) const
{
    auto it = m_itemData->find(index);
           
    if (it == m_itemData->end())
        it = m_itemData->insert(index, ModelIndexInfo());
    
    return it;
}


void Data::forget(const QModelIndex& index)
{
    assert(index.isValid());                         // we cannot forget root node
    auto it = m_itemData->find(index);

    if (it != m_itemData->end())
        m_itemData->erase(it);
}


ModelIndexInfoSet::iterator Data::get(const QPoint& point) const
{
    ModelIndexInfoSet::iterator result = m_itemData->end();

    for(auto it = m_itemData->begin(); it != m_itemData->end(); ++it)
    {
        const ModelIndexInfo& info = **it;

        if (info.getRect().contains(point) && isVisible(it))
        {
            result = it;
            break;
        }
    }

    return result;
}


bool Data::isImage(const QModelIndex& index) const
{
    bool result = false;

    if (index.isValid())
    {
        const QAbstractItemModel* model = index.model();
        const bool has_children = model->hasChildren(index);

        if (!has_children)     //has no children? Leaf (image) or empty node, so still not sure
        {
            QPixmap pixmap = getImage(index);

            result = pixmap.isNull() == false;
        }
        //else - has children so it is node so it is not image :)
    }
    
    return result;
}


QPixmap Data::getImage(const QModelIndex& index) const
{
    const QAbstractItemModel* model = index.model();
    const QVariant decorationRole = model->data(index, Qt::DecorationRole);  //get display role
    const bool directlyConvertable = decorationRole.canConvert<QPixmap>();
    QPixmap pixmap;

    if (directlyConvertable)
        pixmap = decorationRole.value<QPixmap>();
    else
    {
        const bool isIcon = decorationRole.canConvert<QIcon>();

        if (isIcon)
        {
            const QIcon icon = decorationRole.value<QIcon>();
            auto sizes = icon.availableSizes();

            if (sizes.isEmpty() == false)
                pixmap = icon.pixmap(sizes[0]);
        }
    }

    return pixmap;
}


void Data::for_each(std::function<bool(const ModelIndexInfo &)> f) const
{
    ModelIndexInfoSet::const_iterator it = m_itemData->cbegin();
    auto it_end = m_itemData->cend();

    ModelIndexInfo result(QModelIndex());

    for(; it != it_end; ++it)
    {
        const ModelIndexInfo& info = *it;
        const bool cont = f(info);

        if (!cont)
            break;
    }
}


void Data::for_each_visible(std::function<bool(const ModelIndexInfo &)> f) const
{
    for(auto it = m_itemData->cbegin(); it != m_itemData->cend(); ++it)
    {
        const ModelIndexInfo& info = **it;

        bool cont = true;
        if (isVisible(it))
            cont = f(info);

        if (cont == false)
            break;
    }
}


void Data::for_each_visible(std::function<bool(ModelIndexInfoSet::iterator)> f) const
{
    for(auto it = m_itemData->begin(); it != m_itemData->end(); ++it)
    {
        bool cont = true;
        if (isVisible(it))
            cont = f(it);

        if (cont == false)
            break;
    }
}


QModelIndex Data::get(const ModelIndexInfoSet::iterator& it) const
{
    assert(m_model != nullptr);

    ModelIndexInfoSet::iterator parent = it.parent();
    const size_t i = ModelIndexInfoSet::flat_iterator(it).index();

    QModelIndex result;

    if (parent == m_itemData->end())   // one of top items
        result = m_model->index(i, 0);
    else                               // somewhere deep in hierarchy
    {
        QModelIndex parentIdx = get(parent);  // index of parent
        result = m_model->index(i, 0, parentIdx);
    }

    return result;
}


bool Data::isExpanded(const QModelIndex& index) const
{
    ModelIndexInfoSet::iterator infoIt = get(index);
    const ModelIndexInfo& info = **infoIt;
    const bool status = info.expanded;

    return status;
}


bool Data::isExpanded(const ModelIndexInfoSet::iterator& it) const
{
    const ModelIndexInfo& info = **it;
    return info.expanded;
}


bool Data::isExpanded(const ModelIndexInfoSet::const_iterator& it) const
{
    const ModelIndexInfo& info = **it;
    return info.expanded;
}


bool Data::isVisible(const QModelIndex& index) const
{
    QModelIndex parent = index.parent();
    bool result = false;

    if (parent == QModelIndex())    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isVisible(const ModelIndexInfoSet::iterator& it) const
{
    ModelIndexInfoSet::iterator parent = it.parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isVisible(const ModelIndexInfoSet::const_iterator& it) const
{
    ModelIndexInfoSet::const_iterator parent = it.parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


void Data::for_each_recursively(QAbstractItemModel* m, std::function<void(const QModelIndex &, const std::deque<QModelIndex> &)> f, const QModelIndex& first)
{
    std::deque<QModelIndex> c_result = for_each_recursively(m, first, f);
    f(first, c_result);
}


std::deque<QModelIndex> Data::for_each_recursively(QAbstractItemModel* m, const QModelIndex& idx, std::function<void(const QModelIndex &, const std::deque<QModelIndex> &)> f)
{
    std::deque<QModelIndex> result;
    const bool expanded = isExpanded(idx);

    if (expanded)
    {
        const bool fetchMore = m->canFetchMore(idx);
        if (fetchMore)
            m->fetchMore(idx);

        const bool has_children = m->hasChildren(idx);

        if (has_children)
        {
            const int r = m->rowCount(idx);
            for(int i = 0; i < r; i++)
            {
                QModelIndex c = m->index(i, 0, idx);
                result.push_back(c);

                std::deque<QModelIndex> c_result = for_each_recursively(m, c, f);

                f(c, c_result);
            }
        }
    }

    return result;
}


void Data::clear()
{
    m_itemData->clear();
}


const ModelIndexInfoSet& Data::getAll() const
{
    return *m_itemData;
}


ModelIndexInfoSet& Data::getAll()
{
    return *m_itemData;
}


void Data::dump()
{
    /*
    int i = 0;
    for_each([&](const ModelIndexInfo& item)
    {
        std::cout << i++ << ": " << item.index << ", " << item.getRect() << ", " << item.getOverallRect() << ", expanded: " << item.expanded << std::endl;

        return true;
    });

    std::cout << std::endl;
    */
}
