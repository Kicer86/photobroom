/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#include "idx_data.hpp"

#include <assert.h>

#include <QPixmap>
#include <QVariant>

#include <database/iphoto_info.hpp>

#include "idx_data_manager.hpp"


namespace
{
    struct TagValueComparer
    {
        TagValueComparer(const Hierarchy::Level& l): m_compFun(nullptr)
        {
            switch (l.order)
            {
                case Hierarchy::Level::Order::ascending:  m_compFun = &TagValueComparer::ascendingComparer;  break;
                case Hierarchy::Level::Order::descending: m_compFun = &TagValueComparer::descendingComparer; break;
            }
        }

        bool operator() (const QString& l, const QString& r) const
        {
            return (this->*m_compFun)(l, r);
        }

        private:
            bool ascendingComparer(const QString& l, const QString& r) const
            {
                return l < r;
            }

            bool descendingComparer(const QString& l, const QString& r) const
            {
                return l > r;
            }

            typedef bool (TagValueComparer::*CompFun)(const QString& l, const QString& r) const;

            CompFun m_compFun;
    };


    struct IdxDataComparer
    {
        IdxDataComparer(const Hierarchy::Level& l): m_level(l), m_comparer(l) {}

        bool operator() (const IdxData* l, const IdxData* r) const
        {
            assert(l->isNode() == r->isNode());

            const bool result = l->isNode()? compareNodes(l, r): compareLeafs(l, r);

            return result;
        }

        private:
            const Hierarchy::Level& m_level;
            TagValueComparer m_comparer;

            bool compareNodes(const IdxData* l, const IdxData* r) const
            {
                const QString l_val = l->m_data[Qt::DisplayRole].toString();
                const QString r_val = r->m_data[Qt::DisplayRole].toString();

                return m_comparer(l_val, r_val);
            }

            bool compareLeafs(const IdxData* l, const IdxData* r) const
            {
                const QString l_val = getValue(l);
                const QString r_val = getValue(r);

                return m_comparer(l_val, r_val);
            }

            QString getValue(const IdxData* idx) const
            {
                const Tag::TagsList& tags = idx->m_photo->getTags();

                const auto& tag = tags.find(m_level.tagName);

                QString result;
                if (tag != tags.cend())
                    result = tag->second.get();

                return result;
            }
    };
}


IdxData::IdxData(IdxDataManager* model, const QString& name): IdxData(model)
{
    m_data[Qt::DisplayRole] = name;
    
    init();
}


IdxData::IdxData(IdxDataManager* model, const IPhotoInfo::Ptr& photo): IdxData(model)
{
    m_photo = photo;
    m_loaded = FetchStatus::Fetched;

    updateLeafData();
    photo->registerObserver(this);

    init();
}


IdxData::~IdxData()
{
    m_model->idxDataDeleted(this);

    if (m_photo.get() != nullptr)
        m_photo->unregisterObserver(this);

    reset();
}


void IdxData::setNodeFilter(const Database::IFilter::Ptr& filter)
{
    m_filter = filter;
}


void IdxData::setNodeSorting(const Hierarchy::Level& order)
{
    m_order = order;
}


int IdxData::findPositionFor(const IdxData* child) const
{
    IdxDataComparer comparer(m_order);

    const auto pos = std::upper_bound(m_children.cbegin(), m_children.cend(), child, comparer);

    return pos - m_children.cbegin();
}


int IdxData::getPositionOf(const IdxData* child) const
{
    const auto pos = std::find(m_children.cbegin(), m_children.cend(), child);

    assert(pos != m_children.cend());

    return pos - m_children.cbegin();
}


void IdxData::addChild(IdxData* child)
{
    assert(isNode());                        // child (leaf) cannot accept any child
    assert(child->m_parent == nullptr);      // child should not have parent

    const size_t pos = findPositionFor(child);
    m_children.insert(m_children.cbegin() + pos, child);
    child->setParent(this);
}


void IdxData::removeChild(IdxData* child)
{
    takeChild(child);

    delete child;
}


void IdxData::takeChild(IdxData* child)
{
    assert(child->m_parent == this);
    assert(static_cast<unsigned int>(child->getRow()) < m_children.size());

    const int pos = getPositionOf(child);
    m_children.erase(m_children.cbegin() + pos);

    child->setParent(nullptr);
}



void IdxData::reset()
{
    m_model->idxDataReset(this);
    m_loaded = FetchStatus::NotFetched;
    for(IdxData* child: m_children)      //TODO: it may be required to move deletion to another thread (slow deletion may impact gui)
        delete child;

    m_children.clear();
    m_photo.reset();
    m_data.clear();
}


void IdxData::setParent(IdxData* _parent)
{
    m_parent = _parent;
    m_level = _parent ? _parent->m_level + 1 : 0;
}


IdxData* IdxData::parent() const
{
    return m_parent;
}


bool IdxData::isPhoto() const
{
    return m_photo.get() != nullptr;
}


bool IdxData::isNode() const
{
    return m_photo.get() == nullptr;
}


int IdxData::getRow() const
{
    assert(m_parent != nullptr);
    return m_parent->getPositionOf(this);
}


int IdxData::getCol() const
{
    return 0;
}


IdxData* IdxData::findChildWithBadPosition() const
{
    IdxDataComparer comparer(m_order);
    IdxData* result = nullptr;

    for(size_t i = 1; i < m_children.size(); i++)
        if (comparer(m_children[i -1], m_children[i]) == false)
        {
            result = m_children[i - 1];
            break;
        }

    return result;
}


bool IdxData::sortingRequired() const
{
    IdxDataComparer comparer(m_order);

    const bool sorted = std::is_sorted(m_children.cbegin(), m_children.cend(), comparer);
    const bool required = !sorted;

    return required;
}


IdxData::IdxData(IdxDataManager* model) :
    m_children(),
    m_data(),
    m_filter(new Database::EmptyFilter),
    m_order(),
    m_photo(nullptr),
    m_model(model),
    m_level(-1),
    m_loaded(FetchStatus::NotFetched),
    m_parent(nullptr)
{

}


void IdxData::updateLeafData()
{
    m_data[Qt::DisplayRole] = m_photo->getPath();
    m_data[Qt::DecorationRole] = m_photo->getThumbnail();
}


void IdxData::init()
{
    m_model->idxDataCreated(this);
}


void IdxData::photoUpdated(IPhotoInfo *)  //parameter not used as we have only one photo
{
    updateLeafData();
}
