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

#ifndef STAGEDPHOTOSDATAMODEL_HPP
#define STAGEDPHOTOSDATAMODEL_HPP

#include "model_view/db_data_model.hpp"

class QObject;

class StagedPhotosDataModel: public DBDataModel
{
    public:
        StagedPhotosDataModel(QObject *);
        StagedPhotosDataModel(const StagedPhotosDataModel &) = delete;
        ~StagedPhotosDataModel();

        StagedPhotosDataModel& operator=(const StagedPhotosDataModel &) = delete;

        void addPhoto(const QString &);
        void storePhotos();

        // DBDataModel interface:
        virtual std::deque<Database::IFilter::Ptr> getModelSpecificFilters() const override;
};

#endif // STAGEDPHOTOSDATAMODEL_HPP
