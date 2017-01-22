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
#include <QDebug>

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

                void visit(const IdxNodeData *) override
                {
                    assert(isNode(m_l));
                    assert(isNode(m_r));

                    const QVariant l_val = m_l->getData(Qt::DisplayRole);
                    const QVariant r_val = m_r->getData(Qt::DisplayRole);

                    m_result = m_comparer(l_val, r_val);
                }

                void visit(const IdxLeafData *) override
                {
                    assert(isLeaf(m_l));
                    assert(isLeaf(m_r));

                    const IdxLeafData* leftLeafData = static_cast<const IdxLeafData *>(m_l);
                    const IdxLeafData* rightLeafData = static_cast<const IdxLeafData *>(m_r);

                    const QVariant l_val = getValue(leftLeafData);
                    const QVariant r_val = getValue(rightLeafData);

                    m_result = m_comparer(l_val, r_val);
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


IdxData::IdxData(IdxDataManager* model, const IPhotoInfo::Ptr& photo): IdxData(model)
{
    m_photo = photo;
    setStatus(NodeStatus::Fetched);

    QImage img;
    img.load(":/gui/clock.svg");

    m_data[Qt::DisplayRole] = m_photo->getPath();
    m_data[Qt::DecorationRole] = img;
}


IdxData::IdxData(IdxDataManager* model, const Photo::Id &): IdxData(model)
{
    m_photo = IPhotoInfo::Ptr();
    setStatus(NodeStatus::Fetched);

    QImage img;
    img.load(":/gui/clock.svg");

    m_data[Qt::DisplayRole] = "";
    m_data[Qt::DecorationRole] = img;
}


IdxData::~IdxData()
{
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


long IdxData::findPositionFor(const IIdxData* child) const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

    auto begin = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cbegin());
    auto end = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cend());

    const auto pos = std::upper_bound(begin, end, child, comparer);

    return pos - begin;
}


long IdxData::getPositionOf(const IIdxData* child) const
{
    auto begin = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cbegin());
    auto end = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cend());

    const auto pos = std::find(begin, end, child);

    assert(pos != end);

    return pos - begin;
}


IIdxData* IdxData::addChild(IIdxData::Ptr&& child)
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


void IdxData::removeChild(IIdxData* child)
{
    takeChild(child);   // take child returns unique_ptr which is not catched here so child will be deleted
}


void IdxData::removeChildren()
{
    m_children.clear();
}


IIdxData::Ptr IdxData::takeChild(IIdxData* child)
{
    assert(child->parent() == this);
    assert(static_cast<unsigned int>(child->getRow()) < m_children.size());

    const long pos = getPositionOf(child);

    IIdxData::Ptr childPtr = std::move(m_children[pos]);
    m_children.erase(m_children.cbegin() + pos);

    childPtr->setParent(nullptr);

    return childPtr;
}


void IdxData::reset()
{
    m_manager->idxDataReset(this);
    setStatus(NodeStatus::NotFetched);

    for(IIdxData::Ptr& child: m_children)
        m_manager->idxDataDeleted(child.get());

    m_children.clear();
    m_photo.reset();
}


void IdxData::setParent(IIdxData* _parent)
{
    m_parent = _parent;
    m_level = _parent ? _parent->getLevel() + 1 : 0;
}


void IdxData::setStatus(NodeStatus status)
{
    m_data[DBDataModel::NodeStatus] = static_cast<int>(status);
}


IIdxData* IdxData::parent() const
{
    return m_parent;
}


const std::vector<IIdxData::Ptr> & IdxData::getChildren() const
{
    return m_children;
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


IPhotoInfo::Ptr IdxData::getPhoto() const
{
    return m_photo;
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


IIdxData* IdxData::findChildWithBadPosition() const
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


bool IdxData::sortingRequired() const
{
    IdxDataComparer<TagValueComparer> comparer(m_order);

    auto begin = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cbegin());
    auto end = ptr_iterator<std::vector<IIdxData::Ptr>>(m_children.cend());

    const bool sorted = std::is_sorted(begin, end, comparer);
    const bool required = !sorted;

    return required;
}


IdxData::IdxData(IdxDataManager* model) :
    m_children(),
    m_data(),
    m_filter(new Database::EmptyFilter),
    m_order(),
    m_photo(nullptr),
    m_level(std::numeric_limits<std::size_t>::max()),
    m_manager (model),
    m_parent(nullptr)
{
    setStatus(NodeStatus::NotFetched);
}


///////////////////////////////////////////////////////////////////////////////


IdxNodeData::IdxNodeData(IdxDataManager* mgr, const QVariant& name): IdxData(mgr, name)
{

}


void IdxNodeData::visitMe(IIdxDataVisitor* visitor) const
{
    visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////


IdxLeafData::IdxLeafData(IdxDataManager* mgr, const IPhotoInfo::Ptr& photo): IdxData(mgr, photo)
{

}


IdxLeafData::IdxLeafData(IdxDataManager* mgr, const Photo::Id& id): IdxData(mgr, id)
{

}


void IdxLeafData::visitMe(IIdxDataVisitor* visitor) const
{
    visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////


IdxRegularLeafData::IdxRegularLeafData(IdxDataManager* mgr, const IPhotoInfo::Ptr& photo): IdxLeafData(mgr, photo)
{

}


IdxRegularLeafData::~IdxRegularLeafData()
{

}



Photo::Id IdxRegularLeafData::getMediaId() const
{
    return m_photo->getID();
}


QString IdxRegularLeafData::getMediaPath() const
{
    return m_photo->getPath();
}


QSize IdxRegularLeafData::getMediaGeometry() const
{
    return m_photo->getGeometry();
}


Tag::TagsList IdxRegularLeafData::getTags() const
{
    return m_photo->getTags();
}

///////////////////////////////////////////////////////////////////////////////


IdxGroupLeafData::IdxGroupLeafData(IdxDataManager* mgr, const Photo::Id& id): IdxLeafData(mgr, id), m_id(id)
{
}


IdxGroupLeafData::~IdxGroupLeafData()
{
}


Photo::Id IdxGroupLeafData::getMediaId() const
{
    return m_id;
}


QString IdxGroupLeafData::getMediaPath() const
{
    return "";
}


QSize IdxGroupLeafData::getMediaGeometry() const
{
    return QSize(64, 64);
}


Tag::TagsList IdxGroupLeafData::getTags() const
{
    return Tag::TagsList();
}


///////////////////////////////////////////////////////////////////////////////


bool is(const IIdxData* idx, const std::initializer_list<bool>& states)
{
    assert( states.size() == 2);

    bool ofType = false;

    apply_inline_visitor(idx,
                         [&ofType, &states](const IdxLeafData *)  { ofType = *(states.begin() + 0); },
                         [&ofType, &states](const IdxNodeData *)  { ofType = *(states.begin() + 1); });

    return ofType;
}


bool isNode(const IIdxData* idx)
{
    const bool result = is(idx, {false, true});

    return result;
}


bool isLeaf(const IIdxData* idx)
{
    const bool result = is(idx, {true, false});

    return result;
}
