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
        void got(int) override
        {

        }
    };
}

StagedPhotosDataModel::StagedPhotosDataModel(QObject* p): DBDataModel(p)
{
    auto filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filter->flags[IPhotoInfo::FlagsE::StagingArea] = 1;

    const std::deque<Database::IFilter::Ptr> filters( {filter});

    setModelSpecificFilter(filters);
}


StagedPhotosDataModel::~StagedPhotosDataModel()
{

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
        photo->markFlag(IPhotoInfo::FlagsE::StagingArea, 0);
}


void StagedPhotosDataModel::dropPhotos()
{
    auto task = std::make_unique<DropPhotosTask>();
    getDatabase()->exec(std::move(task), getModelSpecificFilters());
}

