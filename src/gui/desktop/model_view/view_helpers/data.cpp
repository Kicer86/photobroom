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


bool Data::isImage(const ModelIndexInfoSet::iterator& it) const
{
    QModelIndex index = get(it);

    bool result = false;

    if (index.isValid())
    {
        const QAbstractItemModel* model = index.model();
        const bool has_children = model->hasChildren(index);

        if (!has_children)     //has no children? Leaf (image) or empty node, so still not sure
        {
            QPixmap pixmap = getImage(it);

            result = pixmap.isNull() == false;
        }
        //else - has children so it is node so it is not image :)
    }

    return result;
}


QPixmap Data::getImage(ModelIndexInfoSet::level_iterator it) const
{
    QModelIndex index = get(it);

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

    ModelIndexInfoSet::level_iterator level_it(it);
    ModelIndexInfoSet::iterator parent = level_it.parent();
    const size_t i = level_it.index();

    QModelIndex result;          //top item in tree == QModelIndex()

    const ModelIndexInfoSet::iterator last = m_itemData->end();
    if (parent != last)
    {
        QModelIndex parentIdx = get(parent);  // index of parent
        result = m_model->index(i, 0, parentIdx);
    }

    return result;
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


bool Data::isVisible(const ModelIndexInfoSet::iterator& it) const
{
    ModelIndexInfoSet::iterator parent = ModelIndexInfoSet::level_iterator(it).parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isVisible(const ModelIndexInfoSet::const_iterator& it) const
{
    ModelIndexInfoSet::const_iterator parent = ModelIndexInfoSet::const_level_iterator(it).parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


const Data::ModelIndexInfoSet& Data::getModel() const
{
    return *m_itemData;
}


Data::ModelIndexInfoSet& Data::getModel()
{
    return *m_itemData;
}


QModelIndex Data::getRightOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    return result;
}


QModelIndex Data::getLeftOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    return result;
}


QModelIndex Data::getTopOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    return result;
}


QModelIndex Data::getBottomOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    return result;
}
