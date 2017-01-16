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

#ifndef IDXDATA_HPP
#define IDXDATA_HPP

#include <QMap>

#include <database/iphoto_info.hpp>
#include <database/filter.hpp>

#include "models/db_data_model.hpp"
#include "models/model_types.hpp"

class QVariant;

class IdxDataManager;

struct IIdxDataVisitor;


struct IIdxData
{
    typedef std::unique_ptr<IIdxData> Ptr;

    virtual ~IIdxData() = default;

    virtual void setParent(IIdxData *) = 0;
    virtual void setStatus(NodeStatus) = 0;
    [[deprecated]] virtual IIdxData* addChild(IIdxData::Ptr&& child) = 0;
    [[deprecated]] virtual void removeChild(IIdxData* child) = 0;
    [[deprecated]] virtual void removeChildren() = 0;
    [[deprecated]] virtual IIdxData::Ptr takeChild(IIdxData* child) = 0;
    virtual void reset() = 0;
    virtual void setNodeFilter(const Database::IFilter::Ptr& filter) = 0;
    virtual void setNodeSorting(const Hierarchy::Level &) = 0;

    [[deprecated]] virtual long findPositionFor(const IIdxData* child) const = 0;
    [[deprecated]] virtual long getPositionOf(const IIdxData* child) const = 0;
    virtual IIdxData* parent() const = 0;
    [[deprecated]] virtual const std::vector<Ptr>& getChildren() const = 0;
    virtual QVariant getData(int) const = 0;
    virtual const Database::IFilter::Ptr& getFilter() const = 0;
    virtual std::size_t getLevel() const = 0;
    [[deprecated]] virtual IPhotoInfo::Ptr getPhoto() const = 0;
    [[deprecated]] virtual Tag::TagsList getTags() const = 0;

    virtual int getRow() const = 0;
    virtual int getCol() const = 0;
    [[deprecated]] virtual NodeStatus status() const = 0;

    [[deprecated]] virtual IIdxData* findChildWithBadPosition() const = 0;
    [[deprecated]] virtual bool sortingRequired() const = 0;

    // visitation:
    virtual void visitMe(IIdxDataVisitor *) const = 0;
};


class IdxData: public IIdxData
{
    public:
        // node constructor
        IdxData(IdxDataManager *, const QVariant& name);

        //leaf constructor
        IdxData(IdxDataManager *, const IPhotoInfo::Ptr &);

        virtual ~IdxData();

        IdxData(const IdxData &) = delete;
        IdxData& operator=(const IdxData &) = delete;

        void setNodeFilter(const Database::IFilter::Ptr& filter) override;
        void setNodeSorting(const Hierarchy::Level &) override;
        long findPositionFor(const IIdxData* child) const override;     // returns position where child matches
        long getPositionOf(const IIdxData* child) const override;       // returns position of children
        IIdxData* addChild(IIdxData::Ptr&& child) override;             // returns pointer to child
        void removeChild(IIdxData* child) override;                     // removes child (memory is released)
        void removeChildren() override;
        IIdxData::Ptr takeChild(IIdxData* child) override;              // function acts as removeChild but does not delete children
        void reset() override;
        void setParent(IIdxData *) override;
        void setStatus(NodeStatus) override;
        IIdxData* parent() const override;

        const std::vector<Ptr>& getChildren() const override;
        QVariant getData(int) const override;
        const Database::IFilter::Ptr& getFilter() const override;
        std::size_t getLevel() const override;
        IPhotoInfo::Ptr getPhoto() const override;
        Tag::TagsList getTags() const override;

        int getRow() const override;
        int getCol() const override;

        NodeStatus status() const override;

        IIdxData* findChildWithBadPosition() const override;            // returns first child which lies in a wrong place
        bool sortingRequired() const override;

    private:
        std::vector<Ptr> m_children;
        QMap<int, QVariant> m_data;
        Database::IFilter::Ptr m_filter;         // define which children match
        Hierarchy::Level m_order;                // defines how to sort children
        IPhotoInfo::Ptr m_photo;                 // null for nodes, photo for photos
        size_t m_level;
        IdxDataManager* m_manager;
        IIdxData* m_parent;

        IdxData(IdxDataManager *);
        void initLeafData();
};


class IdxNodeData: public IdxData
{
    public:
        IdxNodeData(IdxDataManager *, const QVariant& name);
        virtual ~IdxNodeData() = default;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


class IdxLeafData: public IdxData
{
    public:
        IdxLeafData(IdxDataManager *, const IPhotoInfo::Ptr &);
        virtual ~IdxLeafData() = default;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


class IdxGroupData: public IdxData
{
    public:
        IdxGroupData(IdxDataManager *, const Photo::Id &);
        virtual ~IdxGroupData() = default;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


struct IIdxDataVisitor
{
    virtual ~IIdxDataVisitor() = default;

    virtual void visit(const IdxLeafData *) = 0;
    virtual void visit(const IdxNodeData *) = 0;
    virtual void visit(const IdxGroupData *) = 0;
};


template<typename LeafFunctor, typename NodeFunctor, typename GroupFunctor>
class InlineIdxDataVisitor: IIdxDataVisitor
{
    public:
        InlineIdxDataVisitor(LeafFunctor leaf, NodeFunctor node, GroupFunctor group): m_leaf(leaf), m_node(node), m_group(group) {}

        void apply(const IIdxData* i)
        {
            i->visitMe(this);
        }

    private:
        LeafFunctor m_leaf;
        NodeFunctor m_node;
        GroupFunctor m_group;

        void visit(const IdxLeafData* i) override
        {
            m_leaf(i);
        }

        void visit(const IdxNodeData* i) override
        {
            m_node(i);
        }

        void visit(const IdxGroupData* i) override
        {
            m_group(i);
        }
};


template<typename LeafFunctor, typename NodeFunctor, typename GroupFunctor>
void apply_inline_visitor(const IIdxData* i, LeafFunctor leaf, NodeFunctor node, GroupFunctor group)
{
    InlineIdxDataVisitor<LeafFunctor, NodeFunctor, GroupFunctor> visitor(leaf, node, group);
    visitor.apply(i);
}


bool isNode(const IIdxData *);
bool isLeaf(const IIdxData *);


#endif // IDXDATA_HPP
