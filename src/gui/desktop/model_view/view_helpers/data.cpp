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
#include <QModelIndex>


namespace
{
    bool validate(QAbstractItemModel* model, const QModelIndex& index, ModelIndexInfoSet::const_flat_iterator it)
    {
        const size_t it_children = it.children_count();
        const size_t idx_children = model->rowCount(index);
        bool equal = it_children == idx_children;

        if (equal && it_children != 0)
            for(size_t i = 0; i < it_children; i++)
                equal = validate(model, model->index(i, 0, index), it.begin() + i);

        return equal;
    }
}


//////////////////////////////////////////////////////////////////////////////////////

Data::Data(): m_configuration(nullptr), m_itemData(new ModelIndexInfoSet), m_model(nullptr)
{
    setupRoot();
}


Data::~Data()
{

}


void Data::set(QAbstractItemModel* model)
{
    m_model = model;
}


ModelIndexInfoSet::iterator Data::get(const QModelIndex& index)
{
    auto it = m_itemData->find(index);
           
    if (it == m_itemData->end())
        it = m_itemData->insert(index, ModelIndexInfo());
    
    assert(index.isValid() || it->expanded == true);

    return it;
}


ModelIndexInfoSet::const_iterator Data::cfind(const QModelIndex& index) const
{
    auto it = m_itemData->cfind(index);

    return it;
}


ModelIndexInfoSet::iterator Data::find(const QModelIndex& index)
{
    auto it = m_itemData->find(index);

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
        const ModelIndexInfo& info = *it;

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

    ModelIndexInfoSet::flat_iterator flat_it(it);
    ModelIndexInfoSet::iterator parent = flat_it.parent();
    const size_t i = flat_it.index();

    QModelIndex result;          //top item in tree == QModelIndex()

    const ModelIndexInfoSet::iterator last = m_itemData->end();
    if (parent != last)
    {
        QModelIndex parentIdx = get(parent);  // index of parent
        result = m_model->index(i, 0, parentIdx);
    }

    return result;
}


bool Data::isExpanded(const QModelIndex& index) const
{
    ModelIndexInfoSet::const_iterator infoIt = cfind(index);

    bool status = false;
    if (infoIt.valid())
    {
        const ModelIndexInfo& info = *infoIt;
        status = info.expanded;
    }

    return status;
}


bool Data::isExpanded(const ModelIndexInfoSet::iterator& it) const
{
    assert(it.valid());

    const ModelIndexInfo& info = *it;
    return info.expanded;
}


bool Data::isExpanded(const ModelIndexInfoSet::const_iterator& it) const
{
    assert(it.valid());

    const ModelIndexInfo& info = *it;
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
    ModelIndexInfoSet::iterator parent = ModelIndexInfoSet::flat_iterator(it).parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isVisible(const ModelIndexInfoSet::const_iterator& it) const
{
    ModelIndexInfoSet::const_iterator parent = ModelIndexInfoSet::const_flat_iterator(it).parent();
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
    setupRoot();
}


const ModelIndexInfoSet& Data::getAll() const
{
    return *m_itemData;
}


ModelIndexInfoSet& Data::getAll()
{
    return *m_itemData;
}


bool Data::validate() const
{
    return ::validate(m_model, QModelIndex(), m_itemData->cbegin());
}


void Data::setupRoot()
{
    //setup info for index QModelIndex()
    auto it = m_itemData->insert(QModelIndex(), ModelIndexInfo());

    it->expanded = true;
}

