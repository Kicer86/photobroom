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

#include <core/iconfiguration.hpp>

#include "positions_translator.hpp"
#include "models/aphoto_info_model.hpp"


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


void Data::set(APhotoInfoModel* model)
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


void Data::setThumbnailDesiredHeight(int imgSize)
{
    m_thumbHeight = imgSize;
}


const ModelIndexInfo& Data::get(const QModelIndex& index) const
{
    auto it = m_itemData->find(index);
    assert(it != m_itemData->end());

    return *it;
}


ModelIndexInfo& Data::get(const QModelIndex& index)
{
    auto it = m_itemData->find(index);
    assert(it != m_itemData->end());

    return *it;
}


Data::ModelIndexInfoSet::Model::iterator Data::find(const QModelIndex& index)
{
    auto it = m_itemData->find(index);

    return it;
}


Data::ModelIndexInfoSet::Model::iterator Data::get(const QPoint& point) const
{
    ModelIndexInfoSet::Model::iterator result = m_itemData->end();
    PositionsTranslator translator(this);

    for(auto it = m_itemData->begin(); it != m_itemData->end(); ++it)
    {
        const QRect rect = translator.getAbsoluteRect(it);

        if (rect.contains(point) && isVisible(it))
        {
            result = it;
            break;
        }
    }

    return result;
}


bool Data::isImage(const ModelIndexInfoSet::Model::const_iterator& it) const
{
    const QModelIndex index = get(it);

    return isImage(index);
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
            const QVariant decorationRole = model->data(index, Qt::DecorationRole);  //get display role

            result = decorationRole.canConvert<QPixmap>() || decorationRole.canConvert<QIcon>();
        }
        //else - has children so it is node so it is not image :)
    }

    return result;
}


QPixmap Data::getImage(typename ModelIndexInfoSet::Model::const_iterator it) const
{
    const QModelIndex index = get(it);

    return getImage(index);
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


QSize Data::getImageSize(ModelIndexInfoSet::Model::const_iterator it) const
{
    const QModelIndex idx = get(it);

    return getImageSize(idx);
}


QSize Data::getImageSize(const QModelIndex& idx) const
{
    const Photo::Data& details = m_model->getPhotoDetails(idx);

    return details.geometry;
}


QSize Data::getThumbnailSize(ModelIndexInfoSet::Model::const_iterator it) const
{
    const QModelIndex idx = get(it);

    return getThumbnailSize(idx);
}


QSize Data::getThumbnailSize(const QModelIndex& index) const
{
    const QSize size = getImageSize(index);

    const int w = size.width();
    const int h = size.height();

    QSize result(size);

    // is image's size known?
    if (h > 0)
    {
        const double r = static_cast<double>(w) / h;

        if (h != m_thumbHeight)
        {
            const int t_h = m_thumbHeight;
            const int t_w = static_cast<int>(m_thumbHeight * r);

            result = QSize(t_w, t_h);
        }
    }
    else  // unknown image size, use default size
        result = QSize(m_thumbHeight, m_thumbHeight);

    return result;
}


void Data::for_each_visible(std::function<bool(ModelIndexInfoSet::Model::iterator)> f) const
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


QModelIndex Data::get(ModelIndexInfoSet::Model::const_level_iterator it) const
{
    assert(m_model != nullptr);

    ModelIndexInfoSet::Model::const_iterator parent = it.parent();
    const size_t i = it.index();

    QModelIndex result;          //top item in tree == QModelIndex()

    const ModelIndexInfoSet::Model::const_iterator last = m_itemData->end();
    if (parent != last)
    {
        QModelIndex parentIdx = get(parent);  // index of parent
        result = m_model->index(static_cast<int>(i), 0, parentIdx);
    }

    return result;
}


std::vector<QModelIndex> Data::findInRect(const QRect& rect) const
{
    const Data::ModelIndexInfoSet& model = getModel();

    ModelIndexInfoSet::Model::const_level_iterator root = model.begin();

    auto first = root.begin();
    auto last = root.end();

    const std::vector<QModelIndex> result = findInRect(first, last, rect);

    return result;
}



bool Data::isExpanded(const ModelIndexInfoSet::Model::const_iterator& it) const
{
    assert(it.valid());

    const ModelIndexInfo& info = *it;
    return info.expanded;
}


bool Data::isExpanded(const QModelIndex& idx) const
{
    assert(idx.isValid());

    const ModelIndexInfo& info = get(idx);
    return info.expanded;
}


bool Data::isVisible(const ModelIndexInfoSet::Model::const_level_iterator& it) const
{
    ModelIndexInfoSet::Model::const_iterator parent = it.parent();
    bool result = false;

    if (parent.valid() == false)    //parent is on the top of hierarchy? Always visible
        result = true;
    else if (isExpanded(parent) && isVisible(parent))    //parent expanded? and visible?
        result = true;

    return result;
}


bool Data::isVisible(const QModelIndex& idx) const
{
    const QModelIndex parent = idx.parent();
    bool result = false;

    if (parent.isValid() == false)    //parent is on the top of hierarchy? Always visible
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


int Data::getSpacing() const
{
    return m_spacing;
}


int Data::getImageMargin() const
{
    return m_margin;
}


int Data::getThumbnailDesiredHeight() const
{
    return m_thumbHeight;
}


IConfiguration* Data::getConfig()
{
    return m_configuration;
}


QModelIndex Data::getRightOf(const QModelIndex& item) const
{
    assert(item.isValid());
    assert(item.column() == 0);

    QModelIndex result = item;

    const QModelIndex sibling = item.sibling(item.row() + 1, 0);
    const ModelIndexInfo& item_info = get(item);

    if (sibling.isValid())
    {
        const ModelIndexInfo& right_item = get(sibling);

        if (item_info.getPosition().y() == right_item.getPosition().y())  // both at the same y?
            result = sibling;
    }

    return result;
}


QModelIndex Data::getLeftOf(const QModelIndex& item) const
{
    assert(item.isValid());
    assert(item.column() == 0);

    QModelIndex result = item;

    const QModelIndex sibling = item.sibling(item.row() - 1, 0);
    const ModelIndexInfo& item_info = get(item);

    if (sibling.isValid())
    {
        const ModelIndexInfo& right_item = get(sibling);

        if (item_info.getPosition().y() == right_item.getPosition().y())  // both at the same y?
            result = sibling;
    }

    return result;
}


QModelIndex Data::getTopOf(const QModelIndex& item) const
{
    assert(item.isValid());
    assert(item.column() == 0);

    QModelIndex result = item;

    const ModelIndexInfo& item_info = get(item);

    for (QModelIndex sibling = item; sibling.isValid(); sibling = sibling.sibling(sibling.row() - 1, 0))
    {
        const ModelIndexInfo& sibling_item = get(sibling);

        if (sibling_item.getPosition().y() < item_info.getPosition().y())       // is sibling_item in row over item?
            if (sibling_item.getPosition().x() <= item_info.getPosition().x())  // and is exactly over it?
            {
                result = sibling;
                break;
            }
    }

    return result;
}


QModelIndex Data::getBottomOf(const QModelIndex& item) const
{
    assert(item.isValid());
    assert(item.column() == 0);

    QModelIndex result = item;

    const ModelIndexInfo& item_info = get(item);

    for (QModelIndex sibling = item; sibling.isValid(); sibling = sibling.sibling(sibling.row() + 1, 0))
    {
        const ModelIndexInfo& sibling_item = get(sibling);

        if (sibling_item.getPosition().y() > item_info.getPosition().y())       // is sibling_item in row below item?
            if (sibling_item.getPosition().x() >= item_info.getPosition().x())  // and is exactly below it?
            {
                result = sibling;
                break;
            }
    }

    return result;
}


QModelIndex Data::getFirst(const QModelIndex& item) const
{
    const QModelIndex result = item.sibling(0, 0);

    return result;
}


QModelIndex Data::getLast(const QModelIndex& item) const
{
    const int siblings = item.model()->rowCount(item.parent());

    const QModelIndex result = item.sibling(siblings - 1, 0);

    return result;
}


std::vector<QModelIndex> Data::findInRect(ModelIndexInfoSet::Model::const_level_iterator first,
                                         ModelIndexInfoSet::Model::const_level_iterator last,
                                         const QRect& rect) const
{
    std::vector<QModelIndex> result;

    PositionsTranslator translator(this);

    const auto lower_bound = lower_bound_iterator(first, last, rect, [&translator](const ModelIndexInfoSet::Model::const_level_iterator& itemIt, const QRect& value)
    {
        const QRect overallRect = translator.getAbsoluteOverallRect(itemIt);
        const int p1 = overallRect.bottom();
        const int p2 = value.top();

        const bool cmp_res = p1 < p2;

        return cmp_res;
    });

    auto upper_bound = lower_bound;

    while(true)
    {
        const bool bound_invalid = upper_bound == last;
        if (bound_invalid)
            break;

        const QRect item_rect = translator.getAbsoluteOverallRect( upper_bound );
        const bool intersects = rect.intersects(item_rect);

        // item itself is visible? Add it
        if (intersects)
        {
            const QModelIndex modelIdx = get( upper_bound );
            assert(modelIdx.isValid());

            result.push_back(modelIdx);
        }

        // item's children
        if ( upper_bound.children_count() > 0 && isExpanded( upper_bound ))
        {
            const std::vector<QModelIndex> children = findInRect( upper_bound.begin(), upper_bound.end(), rect);

            result.insert(result.end(), children.begin(), children.end());
        }

        const bool nextIsVisible = item_rect.top() < rect.bottom();       // as long as we are visible, our horizontal sibling can be visible too

        // Current item may be invisible (beyond top line), but its children and next sibling may be visible
        if (nextIsVisible)
            ++upper_bound;
        else
            break;
    }

    return result;
}
