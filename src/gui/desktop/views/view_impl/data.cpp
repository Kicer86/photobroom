/*
 * View's high level data structure
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

#include <configuration/iconfiguration.hpp>

#include "positions_translator.hpp"


namespace
{
    // custom std::lower_bound implementation http://en.cppreference.com/w/cpp/algorithm/lower_bound
    // the only difference is that it passes iterator to comparator rather than value
    template<class ForwardIt, class T, class Compare>
    ForwardIt lower_bound_iterator(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        ForwardIt it;
        typename std::iterator_traits<ForwardIt>::difference_type count, step;
        count = std::distance(first,last);

        while (count > 0) {
            it = first;
            step = count / 2;
            std::advance(it, step);
            if (comp(it, value)) {
                first = ++it;
                count -= step + 1;
            }
            else
                count = step;
        }
        return first;
    }
}


Data::Data(): m_itemData(new ModelIndexInfoSet), m_model(nullptr), m_configuration(nullptr), m_spacing(5), m_margin(10), m_thumbHeight(120)
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


void Data::setSpacing(int spacing)
{
    m_spacing = spacing;
}


void Data::setImageMargin(int margin)
{
    m_margin = margin;
}


void Data::setThumbHeight(int imgSize)
{
    m_thumbHeight = imgSize;
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


QModelIndex Data::get(const QPoint& point) const
{
    QModelIndex result;

    PositionsTranslator translator(this);

    try
    {
        for_each_child_deep(m_model, QModelIndex(), [&](const QModelIndex& idx)
        {
            const QRect rect = translator.getAbsoluteRect(idx);

            if (rect.contains(point) && isVisible(idx))
            {
                result = idx;
                throw true;
            }
        });
    }
    catch(bool)
    {
    }

    return result;
}


bool Data::isImage(const QModelIndex& idx) const
{
    bool result = false;

    if (idx.isValid())
    {
        const bool has_children = m_model->hasChildren(idx);

        if (!has_children)     //has no children? Leaf (image) or empty node, so still not sure
        {
            const QVariant decorationRole = m_model->data(idx, Qt::DecorationRole);  //get display role

            result = decorationRole.canConvert<QPixmap>() || decorationRole.canConvert<QIcon>();
        }
        //else - has children so it is node so it is not image :)
    }

    return result;
}


QPixmap Data::getImage(const QModelIndex& idx) const
{
    const QVariant decorationRole = m_model->data(idx, Qt::DecorationRole);  //get display role
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


QSize Data::getThumbnailSize(const QModelIndex& idx) const
{
    QPixmap image = getImage(idx);

    const int w = image.width();
    const int h = image.height();

    const double r = static_cast<double>(w) / h;

    QSize result(w, h);

    if (w > m_thumbHeight || h > m_thumbHeight)
    {
        const int t_h = m_thumbHeight;
        const int t_w = m_thumbHeight * r;

        result = QSize(t_w, t_h);
    }
    return result;
}


QModelIndex Data::get_(const ModelIndexInfoSet::const_iterator& it) const
{
    /*
    assert(m_model != nullptr);

    ModelIndexInfoSet::const_level_iterator level_it(it);
    ModelIndexInfoSet::const_iterator parent = level_it.parent();
    const size_t i = level_it.index();

    QModelIndex result;          //top item in tree == QModelIndex()

    const ModelIndexInfoSet::iterator last = m_itemData->end();
    if (parent != last)
    {
        QModelIndex parentIdx = get(parent);  // index of parent
        result = m_model->index(i, 0, parentIdx);
    }

    return result;
    */
}


std::deque<QModelIndex> Data::findInRect(const QRect& rect) const
{
    const std::deque<QModelIndex> result = findInRect(QModelIndex(), rect);

    return result;
}



bool Data::isExpanded(const QModelIndex& idx) const
{
    assert( idx.isValid() );
    auto it = get(idx);

    const ModelIndexInfo& info = it->second;
    return info.expanded;
}


bool Data::isVisible(const QModelIndex& idx) const
{
    const QModelIndex parent = idx.parent();
    bool result = false;

    if (parent.isValid() == false)                       //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isValid(ModelIndexInfoSet::iterator it) const
{
    return it != m_itemData->end();
}


bool Data::isValid(ModelIndexInfoSet::const_iterator it) const
{
    return it != m_itemData->cend();
}



const Data::ModelIndexInfoSet& Data::getModel() const
{
    return *m_itemData;
}


Data::ModelIndexInfoSet& Data::getModel()
{
    return *m_itemData;
}


QAbstractItemModel* Data::getItemModel() const
{
    return m_model;
}


int Data::getSpacing() const
{
    return m_spacing;
}


int Data::getImageMargin() const
{
    return m_margin;
}


IConfiguration* Data::getConfig()
{
    return m_configuration;
}


QModelIndex Data::getRightOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    const ModelIndexInfoSet::iterator item_it = get(item);
    const QModelIndex right = item.sibling(item.row() + 1, 0);

    if (right.isValid())
    {
        const ModelIndexInfoSet::iterator right_it = get(right);
        const ModelIndexInfo& item_info = item_it->second;
        const ModelIndexInfo& right_item = right_it->second;

        if (item_info.getPosition().y() == right_item.getPosition().y())  // both at the same y?
            result = right;
    }

    return result;
}


QModelIndex Data::getLeftOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    const ModelIndexInfoSet::iterator item_it = get(item);

    if (item.row() > 0)
    {
        const QModelIndex left = item.sibling(item.row() - 1, 0);
        const ModelIndexInfoSet::iterator left_it = get(left);

        const ModelIndexInfo& item_info = item_it->second;
        const ModelIndexInfo& left_item = left_it->second;

        if (item_info.getPosition().y() == left_item.getPosition().y())  // both at the same y?
            result = left;
    }

    return result;
}


QModelIndex Data::getTopOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    assert(!"implement");
    /*
    const ModelIndexInfoSet::iterator item_it = get(item);
    assert(item_it.valid());

    const ModelIndexInfo& item_info = *item_it;

    ModelIndexInfoSet::iterator it = item_it;

    while (true)
    {
        const ModelIndexInfo& sibling_item = *it;

        if (sibling_item.getPosition().y() < item_info.getPosition().y())       // is sibling_item in row over item?
            if (sibling_item.getPosition().x() <= item_info.getPosition().x())  // and is exactly over it?
            {
                result = get(it);
                break;
            }

        if (it.is_first())
            break;
        else
            --it;
    }
    */

    return result;
}


QModelIndex Data::getBottomOf(const QModelIndex& item) const
{
    QModelIndex result = item;

    assert(!"implement");
    /*
    const ModelIndexInfoSet::level_iterator item_it = get(item);
    assert(item_it.valid());

    const ModelIndexInfo& item_info = *item_it;

    for(ModelIndexInfoSet::level_iterator it = item_it; it.valid(); ++it)
    {
        const ModelIndexInfo& sibling_item = *it;

        if (sibling_item.getPosition().y() > item_info.getPosition().y())       // is sibling_item in row below item?
            if (sibling_item.getPosition().x() >= item_info.getPosition().x())  // and is exactly below it?
            {
                result = get(it);
                break;
            }
    }
    */

    return result;
}


QModelIndex Data::getFirst(const QModelIndex& item) const
{
    assert(!"implement");

    /*
    const ModelIndexInfoSet::iterator item_it = get(item);
    assert(item_it.valid());

    ModelIndexInfoSet::level_iterator result = item_it;

    for(; result.is_first() == false; --result);

    const QModelIndex resultIdx = get(result);

    return resultIdx;
    */
    return QModelIndex();
}


QModelIndex Data::getLast(const QModelIndex& item) const
{
    assert(!"implement");

    /*
    const ModelIndexInfoSet::level_iterator item_it = get(item);
    assert(item_it.valid());

    ModelIndexInfoSet::level_iterator result = item_it;

    for (; result.is_last() == false; ++result);

    const QModelIndex resultIdx = get(result);

    return resultIdx;
    */

    return QModelIndex();
}


std::deque<QModelIndex> Data::findInRect(const QModelIndex& parent, const QRect& rect) const
{
    std::deque<QModelIndex> result;

    assert(!"implement");
    /*
    PositionsTranslator translator(this);

    auto bound = lower_bound_iterator(first, last, rect, [&translator](const ModelIndexInfoSet::const_iterator& itemIt, const QRect& value)
    {
        const QRect overallRect = translator.getAbsoluteOverallRect(itemIt);
        const int p1 = overallRect.bottom();
        const int p2 = value.top();

        const bool cmp_res = p1 < p2;

        return cmp_res;
    });

    while(true)
    {
        const bool bound_invalid = bound == last;
        if (bound_invalid)
            break;

        const QRect item_rect = translator.getAbsoluteOverallRect(bound);
        const bool intersects = rect.intersects(item_rect);

        // item itself is visible? Add it
        if (intersects)
        {
            const QModelIndex modelIdx = get(bound);
            assert(modelIdx.isValid());

            result.push_back(modelIdx);
        }

        // item's children
        if (bound.children_count() > 0 && isExpanded(bound))
        {
            const std::deque<QModelIndex> children = findInRect(bound.begin(), bound.end(), rect);

            result.insert(result.end(), children.begin(), children.end());
        }

        const bool nextIsVisible = item_rect.top() < rect.bottom();       // as long as we are visible, our horizontal sibling can be visible too

        // Current item may be invisible (beyond top line), but its children and next sibling may be visible
        if (nextIsVisible)
            ++bound;
        else
            break;
    }
    */

    return result;
}
