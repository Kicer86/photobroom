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


Data::Data(): m_configuration(nullptr), m_itemData(new ModelIndexInfoSet), m_model(nullptr)
{

}


Data::~Data()
{

}


void Data::set(QAbstractItemModel* model)
{
    m_model = model;
    m_itemData->set(model);
}


Data::ModelIndexInfoSet::iterator Data::get(const QModelIndex& index) const
{
    auto it = m_itemData->find(index);
    assert(it != m_itemData->end());

    return it;
}


Data::ModelIndexInfoSet::const_iterator Data::cfind(const QModelIndex& index) const
{
    auto it = m_itemData->cfind(index);

    return it;
}


Data::ModelIndexInfoSet::iterator Data::find(const QModelIndex& index)
{
    auto it = m_itemData->find(index);

    return it;
}


Data::ModelIndexInfoSet::iterator Data::get(const QPoint& point) const
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


bool Data::isImage(const ModelIndexInfoSet::iterator& it) const
{
    return isImage(get(it));
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


QPixmap Data::getImage(ModelIndexInfoSet::flat_iterator it) const
{
    return getImage(get(it));
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


void Data::for_each_recursively(std::function<void(ModelIndexInfoSet::flat_iterator)> f)
{
    for_each_recursively(m_itemData->begin(), f);
    f(m_itemData->begin());
}


void Data::for_each_recursively(ModelIndexInfoSet::flat_iterator it, std::function<void(ModelIndexInfoSet::flat_iterator)> f)
{
    std::deque<QModelIndex> result;
    const bool expanded = isExpanded(it);

    if (expanded)
        for(ModelIndexInfoSet::flat_iterator c_it = it.begin(); c_it.valid(); ++c_it)
        {
            for_each_recursively(c_it, f);

            f(c_it);
        }
}


const Data::ModelIndexInfoSet& Data::getModel() const
{
    return *m_itemData;
}


Data::ModelIndexInfoSet& Data::getModel()
{
    return *m_itemData;
}
