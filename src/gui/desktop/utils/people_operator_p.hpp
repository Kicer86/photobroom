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


class FaceTask: public ITaskExecutor::ITask
{
    public:
        FaceTask(const Photo::Id &, Database::IDatabase *);
        ~FaceTask();

    protected:
        const Photo::Id m_id;
        Database::IDatabase* m_db;

        std::vector<FaceData> fetchFacesFromDb() const;
        QString getPhotoPath() const;
};


class FacesFetcher final: public QObject, public FaceTask
{
        Q_OBJECT

    public:
        FacesFetcher(const Photo::Id &, ICoreFactoryAccessor *, Database::IDatabase *);
        virtual ~FacesFetcher();

        std::string name() const override;
        void perform() override;

    signals:
        void faces(const QVector<FaceData> &) const;

    private:
        ICoreFactoryAccessor* m_coreFactory;
};


class FaceRecognizer final: public QObject, public FaceTask
{
        Q_OBJECT

    public:
        FaceRecognizer(const Photo::Id &, const QRect &, const QString& patterns, ICoreFactoryAccessor *, Database::IDatabase *);
        virtual ~FaceRecognizer();

        std::string name() const override;
        void perform() override;

    private:
        const QRect m_rect;
        const QString m_patterns;
        ICoreFactoryAccessor* m_coreFactory;

        PersonData personData(const Person::Id &) const;
        std::vector<PersonLocation> fetchPeopleFromDb() const;

    signals:
        void recognized(const PersonData &) const;
};


class FetchUnassigned final: public QObject, public ITaskExecutor::ITask
{
        Q_OBJECT

    public:
        FetchUnassigned(const Photo::Id &, Database::IDatabase* db);
        ~FetchUnassigned();

        std::string name() const override;
        void perform() override;

    private:
        const Photo::Id m_id;
        Database::IDatabase* m_db;

    signals:
        void unassigned(const QStringList &) const;
};


class FaceStore final: public FaceTask
{
    public:
        FaceStore(const Photo::Id &,
                  const std::vector<std::pair<FaceData, QString>> &,
                  const QStringList& unknownPeople,
                  Database::IDatabase *,
                  const QString& patterns);
        ~FaceStore();

        std::string name() const override;
        void perform() override;

    private:
        const std::vector<std::pair<FaceData, QString>> m_knownPeople;
        const QStringList m_unknownPeople;
        const QString m_patterns;

        std::vector<PersonData> fetchPeople();
};

#endif // PEOPLEOPERATOR_P_HPP
