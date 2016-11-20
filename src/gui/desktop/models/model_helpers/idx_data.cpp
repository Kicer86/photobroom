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
#include <QDirIterator>
#include <QDebug>

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

        bool operator() (const QVariant& l, const QVariant& r) const
        {
            return (this->*m_compFun)(l, r);
        }

        private:
            bool ascendingComparer(const QVariant& l, const QVariant& r) const
            {
                return l < r;
            }

            bool descendingComparer(const QVariant& l, const QVariant& r) const
            {
                return l > r;
            }

            typedef bool (TagValueComparer::*CompFun)(const QVariant& l, const QVariant& r) const;

            CompFun m_compFun;
    };


    struct RelaxedTagValueComparer
    {
        RelaxedTagValueComparer(const Hierarchy::Level& l): m_compFun(nullptr)
        {
            switch (l.order)
            {
                case Hierarchy::Level::Order::ascending:  m_compFun = &RelaxedTagValueComparer::ascendingComparer;  break;
                case Hierarchy::Level::Order::descending: m_compFun = &RelaxedTagValueComparer::descendingComparer; break;
            }
        }

        bool operator() (const QVariant& l, const QVariant& r) const
        {
            return (this->*m_compFun)(l, r);
        }

    private:
        bool ascendingComparer(const QVariant& l, const QVariant& r) const
        {
            return l <= r;
        }

        bool descendingComparer(const QVariant& l, const QVariant& r) const
        {
            return l >= r;
        }

        typedef bool (RelaxedTagValueComparer::*CompFun)(const QVariant& l, const QVariant& r) const;

        CompFun m_compFun;
    };


    template<typename Comparer>
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
            Comparer m_comparer;

            bool compareNodes(const IdxData* l, const IdxData* r) const
            {
                const QVariant l_val = l->m_data[Qt::DisplayRole];
                const QVariant r_val = r->m_data[Qt::DisplayRole];

                return m_comparer(l_val, r_val);
            }

            bool compareLeafs(const IdxData* l, const IdxData* r) const
            {
                const QVariant l_val = getValue(l);
                const QVariant r_val = getValue(r);

                return m_comparer(l_val, r_val);
            }

            QVariant getValue(const IdxData* idx) const
            {
                const Tag::TagsList& tags = idx->m_photo->getTags();

                const auto& tag = tags.find(m_level.tagName);

                QVariant result;
                if (tag != tags.cend())
                    result = tag->second.get();

                return result;
            }
    };
}


IdxData::IdxData(IdxDataManager* model, const QVariant& name): IdxData(model)
{
    m_data[Qt::DisplayRole] = name;

    init();
}


IdxData::IdxData(IdxDataManager* model, const IPhotoInfo::Ptr& photo): IdxData(model)
{
    m_photo = photo;
    setStatus(NodeStatus::Fetched);

    initLeafData();

    init();
}


IdxData::~IdxData()
{
    m_model->idxDataDeleted(this);

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


long IdxData::findPositionFor(const IdxData* child) const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

    const auto pos = std::upper_bound(m_children.cbegin(), m_children.cend(), child, comparer);

    return pos - m_children.cbegin();
}


long IdxData::getPositionOf(const IdxData* child) const
{
    const auto pos = std::find(m_children.cbegin(), m_children.cend(), child);

    assert(pos != m_children.cend());

    return pos - m_children.cbegin();
}


void IdxData::addChild(IdxData* child)
{
    assert(isNode());                        // child (leaf) cannot accept any child
    assert(child->m_parent == nullptr);      // child should not have parent

    const long pos = findPositionFor(child);
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

    const long pos = getPositionOf(child);
    m_children.erase(m_children.cbegin() + pos);

    child->setParent(nullptr);
}


void IdxData::reset()
{
    m_model->idxDataReset(this);
    setStatus(NodeStatus::NotFetched);

    for(IdxData* child: m_children)      //TODO: it may be required to move deletion to another thread (slow deletion may impact gui)
        delete child;

    m_children.clear();
    m_photo.reset();
}


void IdxData::setParent(IdxData* _parent)
{
    m_parent = _parent;
    m_level = _parent ? _parent->m_level + 1 : 0;
}


void IdxData::setStatus(NodeStatus status)
{
    m_data[DBDataModel::NodeStatus] = static_cast<int>(status);
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
    return static_cast<int>(m_parent->getPositionOf(this));
}


int IdxData::getCol() const
{
    return 0;
}


NodeStatus IdxData::status() const
{
    return static_cast<NodeStatus>(m_data[DBDataModel::NodeStatus].toInt());
}


IdxData* IdxData::findChildWithBadPosition() const
{
    IdxDataComparer<RelaxedTagValueComparer> comparer(m_order);
    IdxData* result = nullptr;

    for(size_t i = 1; i < m_children.size(); i++)
        if (comparer(m_children[i - 1], m_children[i]) == false)
        {
            result = m_children[i - 1];
            break;
        }

    return result;
}


bool IdxData::sortingRequired() const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

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
    m_level(std::numeric_limits<std::size_t>::max()),
    m_parent(nullptr)
{
    setStatus(NodeStatus::NotFetched);
}


void IdxData::initLeafData()
{
    QImage img;
    img.load(":/gui/clock.svg");

    m_data[Qt::DisplayRole] = m_photo->getPath();
    m_data[Qt::DecorationRole] = img;
}


void IdxData::init()
{
    m_model->idxDataCreated(this);
}
