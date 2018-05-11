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
#include <QModelIndex>

#include <core/iconfiguration.hpp>

#include "positions_translator.hpp"
#include "models/aphoto_info_model.hpp"
#include "utils/model_index_utils.hpp"


namespace
{
    template<class T, class Compare>
    QModelIndex lower_bound_iterator(QModelIndex first, const QModelIndex& last, const T& value, Compare comp)
    {
        assert(first.isValid());
        assert(last.isValid());
        assert(first.parent() == last.parent());

        QModelIndex parent = first.parent();

        QModelIndex it;
        std::size_t step = 0;
        std::size_t count = last.row() - first.row();

        while (count > 0)
        {
            it = first;
            step = count / 2;
            it = parent.child(it.row() + step, 0);
            if (comp(it, value))
            {
                it = utils::next(it);
                first = it;
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
    ModelIndexInfo* item = m_itemData->find(index);
    assert(item != nullptr);

    return *item;
}


ModelIndexInfo& Data::get(const QModelIndex& index)
{
    ModelIndexInfo* item = m_itemData->find(index);
    assert(item != nullptr);

    return *item;
}


bool Data::has(const QModelIndex& index) const
{
    ModelIndexInfo* item = m_itemData->find(index);

    return item != nullptr;
}


QModelIndex Data::get(const QPoint& point) const
{
    QModelIndex result;

    PositionsTranslator translator(this);

    QModelIndex toCheck = utils::first(*m_model);

    while(toCheck.isValid())
    {
        const QRect overall_rect = translator.getAbsoluteOverallRect(toCheck);

        if (overall_rect.contains(point))
        {
            assert(isVisible(toCheck));

            const QRect rect = translator.getAbsoluteRect(toCheck);

            if (rect.contains(point))
            {
                result = toCheck;
                break;
            }
            else // Overall contains point, but it is not 'toCheck' item itself.
            {
                if (isExpanded(toCheck))            // expanded? go down
                {
                    toCheck = m_model->index(0, 0, toCheck);
                    continue;
                }
            }
        }

        toCheck = utils::next(toCheck);
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
            const QVariant decorationRole = model->data(index, Qt::DecorationRole);  //get display role

            result = decorationRole.canConvert<QPixmap>() || decorationRole.canConvert<QIcon>();
        }
        //else - has children so it is node so it is not image :)
    }

    return result;
}


QSize Data::getImageSize(const QModelIndex& idx) const
{
    const Photo::Data& details = m_model->getPhotoDetails(idx);

    return details.geometry;
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


std::vector<QModelIndex> Data::findInRect(const QRect& rect) const
{
    const std::vector<QModelIndex> result =
        findInRect(rect, utils::first(*m_model));

    return result;
}


bool Data::isExpanded(const QModelIndex& idx) const
{
    assert(idx.isValid());

    const ModelIndexInfo& info = get(idx);
    return info.expanded;
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


const QAbstractItemModel* Data::getQtModel() const
{
    return m_model;
}


void Data::for_each(const std::function<void(ModelIndexInfo &)>& f)
{
    m_itemData->for_each(f);
}


std::size_t Data::size() const
{
    return m_itemData->size();
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

    const QModelIndex sibling = utils::next(item);
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

    for (QModelIndex sibling = item; sibling.isValid(); sibling = utils::next(sibling))
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


void Data::modelReset()
{
    m_itemData->modelReset();
}


void Data::rowsAboutToBeRemoved(const QModelIndex& p, int f, int t)
{
    m_itemData->rowsAboutToBeRemoved(p, f, t);
}


void Data::rowsInserted(const QModelIndex& p, int f, int t)
{
    m_itemData->rowsInserted(p, f, t);
}


std::vector<QModelIndex> Data::findInRect(const QRect& rect, const QModelIndex& first) const
{
    std::vector<QModelIndex> result;

    PositionsTranslator translator(this);

    for(QModelIndex item = first; item.isValid(); item = utils::next(item))
    {
        const QRect r = translator.getAbsoluteOverallRect(item);

        if (r.intersects(rect))
        {
            result.push_back(item);

            const int children = item.model()->rowCount(item);

            if (children > 0 && isExpanded(item))
            {
                const std::vector<QModelIndex> item_results = findInRect(rect, utils::step_in_next(item));

                std::copy(item_results.cbegin(), item_results.cend(), std::back_inserter(result));
            }
        }
    }

    return result;
}
