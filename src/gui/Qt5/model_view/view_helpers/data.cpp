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

#include <QPixmap>
#include <QDebug>

std::ostream &operator<<(std::ostream &o, const QRect &r)
{
    return o << "QRect: [left: " << r.left () << "; top: " << r.top() << "; right: " << r.right() << "; bottom: " << r.bottom() << "]";
}

std::ostream& operator<<( std::ostream& os, const QModelIndex& idx )
{
    QString str;
    QDebug(&str) << idx;

    os << str.toStdString();

    return os;
}


Data::ModelIndexInfo Data::get(const QModelIndex& index)
{
    auto it = m_itemData.find(index);

    //create if doesn't exist
    if (it == m_itemData.end())
    {
        ModelIndexInfo item(index);
        auto iit = m_itemData.insert(item);

        it = iit.first;
    }

    Data::ModelIndexInfo info = *it;

    return info;
}


const Data::ModelIndexInfo& Data::get(const QPoint& point) const
{
    const Data::ModelIndexInfo* result = &m_invalid;

    for_each([&] (const ModelIndexInfo& info)
    {
        bool cont = true;
        if (info.getPosition().contains(point))
        {
            result = &info;
            cont = false;
        }

        return cont;
    });

    return *result;
}


bool Data::isImage(QAbstractItemModel* model, const QModelIndex& index) const
{
    const bool has_children = model->hasChildren(index);
    bool result = false;

    if (!has_children)     //has no children? Leaf (image) or empty node, so still not sure
    {
        QPixmap pixmap = getImage(model, index);

        result = pixmap.isNull() == false;
    }
    //else - has children so it is node so it is not image :)

    return result;
}


QPixmap Data::getImage(QAbstractItemModel* model, const QModelIndex& index) const
{
    const QVariant decorationRole = model->data(index, Qt::DecorationRole);  //get display role
    const bool convertable = decorationRole.canConvert<QPixmap>();
    QPixmap pixmap;

    if (convertable)
        pixmap = decorationRole.value<QPixmap>();

    return pixmap;
}


void Data::for_each(std::function<bool(const ModelIndexInfo &)> f) const
{
    auto it = m_itemData.get<1>().begin();
    auto it_end = m_itemData.get<1>().end();

    ModelIndexInfo result( (QModelIndex()) );

    for(; it != it_end; ++it)
    {
        const Data::ModelIndexInfo& info = *it;
        const bool cont = f(info);

        if (!cont)
            break;
    }
}


void Data::add(const Data::ModelIndexInfo& info)
{
    auto it = m_itemData.find(info.index);

    if (it == m_itemData.end())
        m_itemData.insert(info);
    else
        m_itemData.replace(it, info);

    dump();
}


void Data::clear()
{
    m_itemData.clear();
}


void Data::dump()
{
    int i = 0;
    for_each([&](const ModelIndexInfo& item)
    {
        std::cout << i++ << ": " << item.index << ", " << item.getPosition() << ", expanded: " << item.expanded << std::endl;

        return true;
    });

    std::cout << std::endl;
}

