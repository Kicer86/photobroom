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

        int getRow() const override;
        int getCol() const override;

        NodeStatus status() const override;

        IIdxData* findChildWithBadPosition() const override;            // returns first child which lies in a wrong place
        bool sortingRequired() const override;

    protected:
        IdxData(IdxDataManager *);

        std::vector<Ptr> m_children;
        QMap<int, QVariant> m_data;
        Database::IFilter::Ptr m_filter;         // define which children match
        Hierarchy::Level m_order;                // defines how to sort children
        size_t m_level;
        IdxDataManager* m_manager;
        IIdxData* m_parent;
};


// base node
class IdxNodeData: public IdxData
{
    public:
        IdxNodeData(IdxDataManager *, const QVariant& name);
        virtual ~IdxNodeData() = default;

    private:
        virtual void visitMe(IIdxDataVisitor *) const override;
};


// base leaf
class IdxLeafData: public IdxData
{
    public:
        IdxLeafData(IdxDataManager *, const IPhotoInfo::Ptr &);
        virtual ~IdxLeafData() = default;

        virtual Photo::Id getMediaId() const;
        virtual QString getMediaPath() const;
        virtual QSize getMediaGeometry() const;
        virtual Tag::TagsList getTags() const;

        IPhotoInfo::Ptr getPhoto() const;

    private:
        IPhotoInfo::Ptr m_photo;
};


// basic leaf imlementation
class IdxRegularLeafData: public IdxLeafData
{
    public:
        IdxRegularLeafData(IdxDataManager *, const IPhotoInfo::Ptr &);
        virtual ~IdxRegularLeafData();

    private:
        virtual void visitMe(IIdxDataVisitor *) const override;
};


// group leaf implementation
class IdxGroupLeafData: public IdxLeafData
{
    public:
        IdxGroupLeafData(IdxDataManager *, const IPhotoInfo::Ptr &);
        virtual ~IdxGroupLeafData();

    private:
        std::deque<IPhotoInfo::Ptr> m_photos;

        virtual void visitMe(IIdxDataVisitor *) const override;
};


struct IIdxDataVisitor
{
    virtual ~IIdxDataVisitor() = default;

    virtual void visit(const IdxNodeData *) = 0;
    virtual void visit(const IdxRegularLeafData *) = 0;
    virtual void visit(const IdxGroupLeafData *) = 0;
};


template<typename NodeFunctor, typename RegularLeafFunctor, typename GroupLeafFunctor>
class InlineIdxDataVisitor: IIdxDataVisitor
{
    public:
        InlineIdxDataVisitor(NodeFunctor node,
                             RegularLeafFunctor regular_leaf,
                             GroupLeafFunctor group_leaf):
            m_node(node),
            m_regular_leaf(regular_leaf),
            m_group_leaf(group_leaf)
        {

        }

        void apply(const IIdxData* i)
        {
            i->visitMe(this);
        }

    private:
        NodeFunctor m_node;
        RegularLeafFunctor m_regular_leaf;
        GroupLeafFunctor m_group_leaf;

        void visit(const IdxNodeData* i) override
        {
            m_node(i);
        }

        void visit(const IdxRegularLeafData* i) override
        {
            m_regular_leaf(i);
        }

        void visit(const IdxGroupLeafData* i) override
        {
            m_group_leaf(i);
        }
};


template<typename NodeFunctor, typename RegularLeafFunctor, typename GroupLeafFunctor>
void apply_inline_visitor(const IIdxData* i, NodeFunctor node, RegularLeafFunctor regular_leaf, GroupLeafFunctor group_leaf)
{
    InlineIdxDataVisitor<NodeFunctor, RegularLeafFunctor, GroupLeafFunctor> visitor(node, regular_leaf, group_leaf);
    visitor.apply(i);
}


bool isNode(const IIdxData *);
bool isLeaf(const IIdxData *);

bool isRegularLeaf(const IIdxData *);
bool isGroupedLeaf(const IIdxData *);

#endif // IDXDATA_HPP
