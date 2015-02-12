/*
 * class used for finding right place in tree for new photos
 * Copyright (C) 2014  Michał Walenciak <email>
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

#include "photos_matcher.hpp"

#include "idx_data_manager.hpp"


struct FiltersMatcher: Database::IFilterVisitor
{
    FiltersMatcher();
    FiltersMatcher(const FiltersMatcher &) = delete;
    virtual ~FiltersMatcher();

    FiltersMatcher& operator=(const FiltersMatcher &) = delete;

    bool doesMatch(const IPhotoInfo::Ptr &, const std::deque<Database::IFilter::Ptr> &);
    bool doesMatch(const IPhotoInfo::Ptr &, const Database::IFilter::Ptr &);

    private:
        bool m_doesMatch;
        IPhotoInfo::Ptr m_photo;

        virtual void visit(Database::EmptyFilter *) override;
        virtual void visit(Database::FilterPhotosWithTag *) override;
        virtual void visit(Database::FilterPhotosWithFlags *) override;
        virtual void visit(Database::FilterPhotosWithSha256 *) override;
        virtual void visit(Database::FilterNotMatchingFilter *) override;
};


FiltersMatcher::FiltersMatcher(): m_doesMatch(false), m_photo(nullptr)
{

}


FiltersMatcher::~FiltersMatcher()
{

}


bool FiltersMatcher::doesMatch(const IPhotoInfo::Ptr& photoInfo, const std::deque<Database::IFilter::Ptr>& filters)
{
    m_doesMatch = false;
    m_photo = photoInfo;

    for(const Database::IFilter::Ptr& filter: filters)
    {
        filter->visitMe(this);

        if (m_doesMatch == false)
            break;
    }

    m_photo.reset();
    return m_doesMatch;
}


bool FiltersMatcher::doesMatch(const IPhotoInfo::Ptr& photoInfo, const Database::IFilter::Ptr& filter)
{
    m_doesMatch = false;
    m_photo = photoInfo;

    filter->visitMe(this);

    m_photo = nullptr;
    return m_doesMatch;
}


void FiltersMatcher::visit(Database::EmptyFilter *)
{
    m_doesMatch = true;
}


void FiltersMatcher::visit(Database::FilterPhotosWithTag* filter)
{
    const Tag::TagsList& tagsList = m_photo->getTags();

    auto it = tagsList.find(filter->tagName);
    bool result = it != tagsList.end();

    if (result && filter->tagValue.is_initialized())
    {
        const std::set<QString>& vals = it->second.getAll();

        result = vals.find(*filter->tagValue) != vals.end();
    }

    m_doesMatch = result;
}


void FiltersMatcher::visit(Database::FilterPhotosWithFlags* filter)
{
    bool status = true;

    switch (filter->mode)
    {
        case Database::FilterPhotosWithFlags::Mode::And: status = true;  break;
        case Database::FilterPhotosWithFlags::Mode::Or:  status = false; break;
    }

    for(const auto& it: filter->flags)
    {
        const bool partial_result = m_photo->getFlag(it.first) == it.second;

        switch (filter->mode)
        {
            case Database::FilterPhotosWithFlags::Mode::And:
                status &= partial_result;
                break;

            case Database::FilterPhotosWithFlags::Mode::Or:
                status |= partial_result;
                break;
        }
    }

    m_doesMatch = status;
}


void FiltersMatcher::visit(Database::FilterPhotosWithSha256* filter)
{
    assert(filter->sha256.empty() == false);
    const bool status = m_photo->getHash() == filter->sha256;

    m_doesMatch = status;
}


void FiltersMatcher::visit(Database::FilterNotMatchingFilter* filter)
{
    FiltersMatcher matcher;
    const bool matches = matcher.doesMatch(m_photo, filter->filter);

    m_doesMatch = !matches;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


PhotosMatcher::PhotosMatcher(): m_idxDataManager(nullptr), m_dbDataModel(nullptr)
{

}


PhotosMatcher::~PhotosMatcher()
{

}


void PhotosMatcher::set(IdxDataManager* manager)
{
    m_idxDataManager = manager;
}


void PhotosMatcher::set(DBDataModel* model)
{
    m_dbDataModel = model;
}


bool PhotosMatcher::doesMatchModelFilters(const IPhotoInfo::Ptr& photoInfo) const
{
    std::deque<Database::IFilter::Ptr> filters = m_dbDataModel->getModelSpecificFilters();

    FiltersMatcher matcher;
    const bool result = matcher.doesMatch(photoInfo, filters);

    return result;
}


bool PhotosMatcher::doesMatchFilter(const IPhotoInfo::Ptr& photoInfo, const Database::IFilter::Ptr& filter)
{
    FiltersMatcher matcher;
    const bool result = matcher.doesMatch(photoInfo, filter);

    return result;
}


IdxData* PhotosMatcher::findParentFor(const IPhotoInfo::Ptr& photoInfo) const
{
    return findParentFor(photoInfo, true);
}


IdxData* PhotosMatcher::findCloserAncestorFor(const IPhotoInfo::Ptr& photoInfo) const
{
    return findParentFor(photoInfo, false);
}


IdxData* PhotosMatcher::findParentFor(const IPhotoInfo::Ptr& photoInfo, bool exact) const
{
    const size_t depth = m_idxDataManager->getHierarchy().levels.size();
    IdxData* result = nullptr;
    IdxData* root = m_idxDataManager->getRoot();
    std::deque<IdxData *> toCheck = { root };
    FiltersMatcher matcher;

    while (toCheck.empty() == false)
    {
        IdxData* check = toCheck.front();
        toCheck.pop_front();

        assert(check->isNode());

        const Database::IFilter::Ptr& filter = check->m_filter;
        const bool matches = matcher.doesMatch(photoInfo, filter);

        if (matches)                         //does match - yeah
        {
            if (exact == false)              //for non exact match
                result = check;

            if (check->m_level < depth)      //go thru children
                toCheck.insert(toCheck.end(), check->m_children.begin(), check->m_children.end());
            else
            {
                result = check;              //save result
                break;                       //and quit. We've got best result
            }
        }
    }

    return result;
}
