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

#include "staging_data_model.hpp"

#include <memory>

#include "model_view/idx_data.hpp"

StagingDataModel::StagingDataModel(QObject* p): DBDataModel(p)
{

}


StagingDataModel::~StagingDataModel()
{

}


bool StagingDataModel::addPhoto(const PhotoInfo::Ptr& photoInfo)
{
    photoInfo->markStagingArea();   //mark photo as being in staging area

    IdxData& root = DBDataModel::getRootIdxData();
    const int row = root.m_children.size();

    beginInsertRows(QModelIndex(), row, row);
    root.addChild(photoInfo);                   //TODO: smarter algorithm?
    endInsertRows();

    DBDataModel::updatePhotoInDB(photoInfo);

    return true;
}


std::vector<Database::IFilter::Ptr> StagingDataModel::getModelSpecificFilters() const
{
    auto filter = std::make_shared<Database::FilterFlags>();
    filter->stagingArea = true;

    const std::vector<Database::IFilter::Ptr> result({filter});

    return result;
}
