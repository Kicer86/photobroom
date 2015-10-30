/*
 * Small modification od DBDataModel for staging purposes.
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

#include "staged_photos_data_model.hpp"

#include <memory>


namespace
{
    struct AddPhotoTask: Database::AStorePhotoTask
    {
        void got(bool status) override
        {
            assert(status);
        }
    };

    struct DropPhotosTask: Database::ADropPhotosTask
    {
        void got(const std::deque<IPhotoInfo::Ptr> &) override
        {

        }
    };
}

StagedPhotosDataModel::StagedPhotosDataModel(QObject* p): DBDataModel(p)
{
    auto filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filter->flags[Photo::FlagsE::StagingArea] = 1;

    const std::deque<Database::IFilter::Ptr> filters( {filter});

    setModelSpecificFilter(filters);
}


StagedPhotosDataModel::~StagedPhotosDataModel()
{

}


bool StagedPhotosDataModel::isPhoto(const QModelIndex& idx) const
{
    const IPhotoInfo::Ptr photo = getPhoto(idx);
    return photo.get() != nullptr;
}


void StagedPhotosDataModel::addPhoto(const QString& path)
{
    auto task = std::make_unique<AddPhotoTask>();
    getDatabase()->exec(std::move(task), path);
}


void StagedPhotosDataModel::storePhotos()
{
    deepFetch(QModelIndex());   //fetch root
    const std::vector<IPhotoInfo::Ptr> photos = getPhotos();

    for(const IPhotoInfo::Ptr& photo: photos)
        photo->markFlag(Photo::FlagsE::StagingArea, 0);
}


void StagedPhotosDataModel::dropPhoto(const QModelIndex& index)
{
    const auto photo = getPhoto(index);
    const auto idFilter = std::make_shared<Database::FilterPhotosWithId>();
    idFilter->filter = photo->getID();

    std::deque<Database::IFilter::Ptr> filters = getModelSpecificFilters();
    filters.push_back(idFilter);

    auto task = std::make_unique<DropPhotosTask>();
    getDatabase()->exec(std::move(task), filters);
}


void StagedPhotosDataModel::dropPhotos()
{
    auto task = std::make_unique<DropPhotosTask>();
    getDatabase()->exec(std::move(task), getModelSpecificFilters());
}

