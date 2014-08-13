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

    private:
        bool m_doesMatch;
        IPhotoInfo* m_photo;

        virtual void visit(Database::FilterEmpty *) override;
        virtual void visit(Database::FilterDescription *) override;
        virtual void visit(Database::FilterFlags *) override;
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
    m_photo = photoInfo.get();

    for(const Database::IFilter::Ptr& filter: filters)
    {
        filter->visitMe(this);

        if (m_doesMatch == false)
            break;
    }

    m_photo = nullptr;
    return m_doesMatch;
}


void FiltersMatcher::visit(Database::FilterEmpty *)
{

}


void FiltersMatcher::visit(Database::FilterDescription* filter)
{
    bool result = false;

    const std::shared_ptr<ITagData> tags = m_photo->getTags();
    const ITagData::TagsList tagsList = tags->getTags();

    auto it = tagsList.find(filter->tagName);

    if (it != tagsList.end())
    {
        const std::set<TagValueInfo>& vals = it->second;

        result = vals.find(filter->tagValue) != vals.end();
    }

    m_doesMatch = result;
}


void FiltersMatcher::visit(Database::FilterFlags*)
{

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


IdxData* PhotosMatcher::findParentFor(const IPhotoInfo::Ptr&) const
{

}
