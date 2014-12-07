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


void ModelIndexInfo::setRect(const QRect& r)
{
    rect = r;
    overallRect = QRect();          // not valid anymore
}


void ModelIndexInfo::setOverallRect(const QRect& r)
{
    overallRect = r;
}


const QRect& ModelIndexInfo::getRect() const
{
    return rect;
}


const QRect& ModelIndexInfo::getOverallRect() const
{
    return overallRect;
}


void ModelIndexInfo::cleanRects()
{
    rect = QRect();
    overallRect = QRect();
}


ModelIndexInfo::ModelIndexInfo(const QModelIndex& idx) : index(idx), expanded(false), rect(), overallRect()
{
}


//////////////////////////////////////////////////////////////////////////////////////


ModelIndexInfo Data::get(const QModelIndex& index) const noexcept
{
    const auto it = m_itemData.find(index);
    const ModelIndexInfo info = it != m_itemData.end()? *it: ModelIndexInfo(index);

    return info;
}


void Data::forget(const QModelIndex& index)
{
    assert(index.isValid());                         // we cannot forget root node
    auto it = m_itemData.find(index);

    if (it != m_itemData.end())
        m_itemData.erase(it);
}


const ModelIndexInfo& Data::get(const QPoint& point) const
{
    const ModelIndexInfo* result = &m_invalid;

    for_each([&] (const ModelIndexInfo& info)
    {
        bool cont = true;
        if (info.getRect().contains(point) && isVisible(info.index))
        {
            result = &info;
            cont = false;
        }

        return cont;
    });

    return *result;
}


bool Data::isImage(const QModelIndex& index) const
{
    const QAbstractItemModel* model = index.model();
    const bool has_children = model->hasChildren(index);
    bool result = false;

    if (!has_children)     //has no children? Leaf (image) or empty node, so still not sure
    {
        QPixmap pixmap = getImage(index);

        result = pixmap.isNull() == false;
    }
    //else - has children so it is node so it is not image :)

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
    auto it = m_itemData.get<1>().begin();
    auto it_end = m_itemData.get<1>().end();

    ModelIndexInfo result( (QModelIndex()) );

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
    for_each([&](const ModelIndexInfo& info)
    {
        bool cont = true;
        if (isVisible(info.index))
            cont = f(info);

        return cont;
    });
}


bool Data::isExpanded(const QModelIndex& index) const noexcept
{
    bool status = true;               //for top root return true
    if (index.isValid())
    {
        const ModelIndexInfo& info = get(index);

        status = info.expanded;
    }

    return status;
}


bool Data::isVisible(const QModelIndex& index) const noexcept
{
    QModelIndex parent = index.parent();
    bool result = false;

    if (parent == QModelIndex())    //parent is on the top of hierarchy? Always visible
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


void Data::update(const ModelIndexInfo& info)
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


const Data::ModelIndexInfoSet& Data::getAll() const noexcept
{
    return m_itemData;
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
