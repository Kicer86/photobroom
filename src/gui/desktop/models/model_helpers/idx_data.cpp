/*
 * Base DBDataModel's item.
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

#include <database/iphoto_info.hpp>

#include "idx_data_manager.hpp"

#include <core/ptr_iterator.hpp>


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
        IdxDataComparer(const Hierarchy::Level& l): m_level(l) {}

        bool operator() (const IIdxData* l, const IIdxData* r) const
        {
            const bool result = InternalComparer(m_level, l, r).compare();

            return result;
        }

        private:
            const Hierarchy::Level& m_level;

            struct InternalComparer: IIdxDataVisitor
            {
                const IIdxData* m_l;
                const IIdxData* m_r;
                Comparer m_comparer;
                const Hierarchy::Level& m_level;
                bool m_result;

                InternalComparer(const Hierarchy::Level& level, const IIdxData* l, const IIdxData* r):
                    m_l(l),
                    m_r(r),
                    m_comparer(level),
                    m_level(level),
                    m_result(false)
                {

                }

                InternalComparer(const InternalComparer &) = delete;
                InternalComparer& operator=(const InternalComparer &) = delete;

                bool compare()
                {
                    m_l->visitMe(this);

                    return m_result;
                }

                void leaf()
                {
                    assert(isLeaf(m_l));
                    assert(isLeaf(m_r));

                    const IdxLeafData* leftLeafData = static_cast<const IdxLeafData *>(m_l);
                    const IdxLeafData* rightLeafData = static_cast<const IdxLeafData *>(m_r);

                    const QVariant l_val = getValue(leftLeafData);
                    const QVariant r_val = getValue(rightLeafData);

                    m_result = m_comparer(l_val, r_val);
                }

                void node()
                {
                    assert(isNode(m_l));
                    assert(isNode(m_r));

                    const QVariant l_val = m_l->getData(Qt::DisplayRole);
                    const QVariant r_val = m_r->getData(Qt::DisplayRole);

                    m_result = m_comparer(l_val, r_val);
                }

                void visit(const IdxNodeData *) override
                {
                    node();
                }

                void visit(const IdxRegularLeafData *) override
                {
                    leaf();
                }

                void visit(const IdxGroupLeafData *) override
                {
                    leaf();
                }

                QVariant getValue(const IdxLeafData* idx) const
                {
                    const Tag::TagsList tags = idx->getTags();

                    const auto& tag = tags.find(m_level.tagName);

                    QVariant result;
                    if (tag != tags.cend())
                        result = tag->second.get();

                    return result;
                }
            };
    };
}


IdxData::IdxData(IdxDataManager* model, const QVariant& name): IdxData(model)
{
    m_data[Qt::DisplayRole] = name;
}


IdxData::IdxData(IdxDataManager* model, const Photo::Data& photo): IdxData(model)
{
    setStatus(NodeStatus::Fetched);

    QImage img;
    img.load(":/gui/clock.svg");

    m_data[Qt::DisplayRole] = photo.path;
    m_data[Qt::DecorationRole] = img;
}


IdxData::~IdxData()
{

}


void IdxData::setNodeFilter(const Database::IFilter::Ptr& filter)
{
    m_filter = filter;
}


void IdxData::setNodeSorting(const Hierarchy::Level& order)
{
    m_order = order;
}



void IdxData::reset()
{
    m_manager->idxDataReset(this);
}


void IdxData::setParent(IdxNodeData* _parent)
{
    m_parent = _parent;
    m_level = _parent ? _parent->getLevel() + 1 : 0;
}


void IdxData::setStatus(NodeStatus status)
{
    m_data[DBDataModel::NodeStatus] = static_cast<int>(status);
}


IdxNodeData* IdxData::parent() const
{
    return m_parent;
}


QVariant IdxData::getData(int role) const
{
    return m_data[role];
}


const Database::IFilter::Ptr& IdxData::getFilter() const
{
    return m_filter;
}


std::size_t IdxData::getLevel() const
{
    return m_level;
}


int IdxData::getRow() const
{
    assert(m_parent != nullptr);
    assert(isNode(m_parent));

    IdxNodeData* parentNode = static_cast<IdxNodeData *>(m_parent);

    return static_cast<int>(parentNode->getPositionOf(this));
}


int IdxData::getCol() const
{
    return 0;
}


NodeStatus IdxData::status() const
{
    return static_cast<NodeStatus>(m_data[DBDataModel::NodeStatus].toInt());
}


IdxData::IdxData(IdxDataManager* model):
    m_data(),
    m_filter(new Database::EmptyFilter),
    m_order(),
    m_level(std::numeric_limits<std::size_t>::max()),
    m_manager (model),
    m_parent(nullptr)
{
    setStatus(NodeStatus::NotFetched);
}


///////////////////////////////////////////////////////////////////////////////


IdxNodeData::IdxNodeData(IdxDataManager* mgr, const QVariant& name):
    IdxData(mgr, name),
    m_children()
{

}


IdxNodeData::~IdxNodeData()
{
    m_manager->idxDataDeleted(this);
}


IIdxData* IdxNodeData::addChild(IIdxData::Ptr&& child)
{
    assert(isNode(this));                    // child (leaf) cannot accept any child
    assert(child->parent() == nullptr);      // child should not have parent

    const long pos = findPositionFor(child.get());
    child->setParent(this);
    m_children.insert(m_children.cbegin() + pos, std::move(child));

    IIdxData* item = m_children[pos].get();
    m_manager->idxDataCreated(item);

    return item;
}


void IdxNodeData::removeChild(IIdxData* child)
{
    takeChild(child);   // take child returns unique_ptr which is not catched here so child will be deleted
}


void IdxNodeData::removeChildren()
{
    m_children.clear();
}


IIdxData::Ptr IdxNodeData::takeChild(IIdxData* child)
{
    assert(child->parent() == this);
    assert(static_cast<unsigned int>(child->getRow()) < m_children.size());

    const long pos = getPositionOf(child);

    IIdxData::Ptr childPtr = std::move(m_children[pos]);
    m_children.erase(m_children.cbegin() + pos);

    childPtr->setParent(nullptr);

    return childPtr;
}


const std::vector<IIdxData::Ptr>& IdxNodeData::getChildren() const
{
    return m_children;
}


long IdxNodeData::getPositionOf(const IIdxData* child) const
{
    //convert unique_ptrs vector to raw pointers vector
    std::vector<const IIdxData *> children;

    for (const auto& c : m_children)
        children.push_back(c.get());

    auto begin = children.cbegin();
    auto end = children.cend();

    const auto pos = std::find(begin, end, child);

    assert(pos != end);

    return pos - begin;
}


long IdxNodeData::findPositionFor(const IIdxData* child) const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

    //convert unique_ptrs vector to raw pointers vector
    std::vector<const IIdxData *> children;

    for (const auto& c : m_children)
        children.push_back(c.get());

    auto begin = children.cbegin();
    auto end = children.cend();
    
    const auto pos = std::upper_bound(begin, end, child, comparer);

    return pos - begin;
}



IIdxData* IdxNodeData::findChildWithBadPosition() const
{
    IdxDataComparer<RelaxedTagValueComparer> comparer(m_order);
    IIdxData* result = nullptr;

    for(size_t i = 1; i < m_children.size(); i++)
        if (comparer(m_children[i - 1].get(), m_children[i].get()) == false)
        {
            result = m_children[i - 1].get();
            break;
        }

    return result;
}


bool IdxNodeData::sortingRequired() const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

    //convert unique_ptrs vector to raw pointers vector
    std::vector<const IIdxData *> children;

    for (const auto& c : m_children)
        children.push_back(c.get());

    auto begin = children.cbegin();
    auto end = children.cend();

    const bool sorted = std::is_sorted(begin, end, comparer);
    const bool required = !sorted;

    return required;
}


void IdxNodeData::reset()
{
    IdxData::reset();

    setStatus(NodeStatus::NotFetched);
    m_children.clear();
}


void IdxNodeData::visitMe(IIdxDataVisitor* visitor) const
{
    visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////


IdxLeafData::IdxLeafData(IdxDataManager* mgr, const Photo::Data& photo):
    IdxData(mgr, photo),
    m_photo(photo)
{

}


IdxLeafData::~IdxLeafData()
{
    m_manager->idxDataDeleted(this);
}


const Photo::Id& IdxLeafData::getMediaId() const
{
    return m_photo.id;
}


const QString& IdxLeafData::getMediaPath() const
{
    return m_photo.path;
}


const QSize& IdxLeafData::getMediaGeometry() const
{
    return m_photo.geometry;
}


const Tag::TagsList& IdxLeafData::getTags() const
{
    return m_photo.tags;
}


const Photo::Data& IdxLeafData::getPhoto() const
{
    return m_photo;
}


void IdxLeafData::update(const Photo::Data& data)
{
    assert(data.id == m_photo.id);

    m_photo = data;
}


///////////////////////////////////////////////////////////////////////////////


IdxRegularLeafData::IdxRegularLeafData(IdxDataManager* mgr, const Photo::Data& photo): IdxLeafData(mgr, photo)
{

}


IdxRegularLeafData::~IdxRegularLeafData()
{

}


void IdxRegularLeafData::visitMe(IIdxDataVisitor* visitor) const
{
    visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////


IdxGroupLeafData::IdxGroupLeafData(IdxDataManager* mgr, const Photo::Data& photoData):
    IdxLeafData(mgr, photoData),
    m_photos()
{

}


IdxGroupLeafData::~IdxGroupLeafData()
{
}


void IdxGroupLeafData::visitMe(IIdxDataVisitor* visitor) const
{
    visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////


bool is(const IIdxData* idx, const std::initializer_list<bool>& states)
{
    assert( states.size() == 3);

    bool ofType = false;

    apply_inline_visitor(idx,
                         [&ofType, &states](const IdxNodeData *)        { ofType = *(states.begin() + 0); },
                         [&ofType, &states](const IdxRegularLeafData *) { ofType = *(states.begin() + 1); },
                         [&ofType, &states](const IdxGroupLeafData *)   { ofType = *(states.begin() + 2); }
                        );

    return ofType;
}


bool isNode(const IIdxData* idx)
{
    const bool result = is(idx, {true, false, false});

    return result;
}


bool isLeaf(const IIdxData* idx)
{
    const bool result = is(idx, {false, true, true});

    return result;
}


bool isRegularLeaf(const IIdxData* idx)
{
    const bool result = is(idx, {false, true, false});

    return result;
}


bool isGroupedLeaf(const IIdxData* idx)
{
    const bool result = is(idx, {false, false, true});

    return result;
}
