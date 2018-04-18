/*
 * Access information about people from db and photo itself.
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

#ifndef PEOPLEOPERATOR_HPP
#define PEOPLEOPERATOR_HPP

#include <QObject>
#include <QVector>
#include <QRect>

#include <core/callback_ptr.hpp>
#include <core/itask_executor.hpp>
#include <database/photo_data.hpp>
#include <database/person_data.hpp>


namespace Database
{
    struct IDatabase;
}

struct ICoreFactoryAccessor;


class PeopleOperator final: public QObject
{
        Q_OBJECT

    public:
        PeopleOperator(Database::IDatabase *, ICoreFactoryAccessor *);
        ~PeopleOperator();

        // Locate faces on given photo.
        void fetchFaces(const Photo::Id &) const;

    signals:
        void faces(const Photo::Id &, const QVector<QRect> &) const;

    private:
        safe_callback_ctrl m_callCtrl;
        Database::IDatabase* m_db;
        ICoreFactoryAccessor* m_coreFactory;
};


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

#endif // PEOPLEOPERATOR_HPP
