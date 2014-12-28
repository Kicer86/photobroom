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

//TODO: remove
#include "model_view/model_helpers/idx_data.hpp"

namespace
{
    struct AddPhotoTask: Database::IStorePhotoTask
    {
        virtual void got(bool status)
        {
            assert(status);
        }
    };
}

StagedPhotosDataModel::StagedPhotosDataModel(QObject* p): DBDataModel(p)
{

}


StagedPhotosDataModel::~StagedPhotosDataModel()
{

}


void StagedPhotosDataModel::addPhoto(const QString& path)
{
    std::unique_ptr<Database::IStorePhotoTask> task(new AddPhotoTask);
    getDatabase()->exec(std::move(task), path);
}


void StagedPhotosDataModel::storePhotos()
{
    deepFetch(QModelIndex());   //fetch root
    const std::vector<IPhotoInfo::Ptr> photos = getPhotos();

    for(const IPhotoInfo::Ptr& photo: photos)
        photo->markFlag(IPhotoInfo::FlagsE::StagingArea, 0);
}


std::deque<Database::IFilter::Ptr> StagedPhotosDataModel::getModelSpecificFilters() const
{
    auto filter = std::make_shared<Database::FilterPhotosWithFlags>();
    filter->stagingArea = 1;

    const std::deque<Database::IFilter::Ptr> result( {filter});

    return result;
}
