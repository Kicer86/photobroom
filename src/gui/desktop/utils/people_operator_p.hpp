/*
 * Private details of PeopleOperator
 * Copyright (C) 2018  Michał Walenciak <Kicer86@gmail.com>
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
 */

#ifndef PEOPLEOPERATOR_P_HPP
#define PEOPLEOPERATOR_P_HPP

#include <QObject>
#include <QVector>
#include <QRect>

#include <core/itask_executor.hpp>
#include <database/photo_data.hpp>
#include <database/person_data.hpp>


namespace Database
{
    struct IDatabase;
}

struct ICoreFactoryAccessor;


class FacesFetcher final: public QObject, public ITaskExecutor::ITask
{
        Q_OBJECT

    public:
        FacesFetcher(const Photo::Id &, ICoreFactoryAccessor *, Database::IDatabase *);
        virtual ~FacesFetcher();

        std::string name() const override;
        void perform() override;
        std::vector<PersonLocation> fetchFacesFromDb() const;
        QString getPhotoPath() const ;

        const Photo::Id m_id;
        ICoreFactoryAccessor* m_coreFactory;
        Database::IDatabase* m_db;

    signals:
        void faces(const QVector<QRect> &) const;
};

#endif // PEOPLEOPERATOR_P_HPP
