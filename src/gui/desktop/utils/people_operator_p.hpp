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

#include "people_operator.hpp"


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

        std::vector<QRect> fetchFacesFromDb() const;
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
        void faces(const QVector<QRect> &) const;

    private:
        ICoreFactoryAccessor* m_coreFactory;
};


class FaceRecognizer final: public QObject, public FaceTask
{
        Q_OBJECT

    public:
        FaceRecognizer(const PeopleOperator::FaceLocation &, const QString& patterns, ICoreFactoryAccessor *, Database::IDatabase *);
        virtual ~FaceRecognizer();

        std::string name() const override;
        void perform() override;

    private:
        const PeopleOperator::FaceLocation m_data;
        const QString m_patterns;
        ICoreFactoryAccessor* m_coreFactory;

        PersonName personData(const Person::Id &) const;
        std::vector<PersonInfo> fetchPeopleFromDb() const;
        bool wasAnalyzed() const;

    signals:
        void recognized(const QRect &, const PersonName &) const;
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
        void unassigned(const Photo::Id &, const QStringList &) const;
};


class FaceStore final: public FaceTask
{
    public:
        FaceStore(const Photo::Id &,
                  const std::vector<PeopleOperator::FaceInfo> &,
                  const QStringList& unknownPeople,
                  Database::IDatabase *,
                  const QString& patterns);
        ~FaceStore();

        std::string name() const override;
        void perform() override;

    private:
        const std::vector<PeopleOperator::FaceInfo> m_knownPeople;
        const QStringList m_unknownPeople;
        const QString m_patterns;

        std::vector<PersonName> fetchPeople();
};


class ModelFaceStore final: public FaceTask
{
    public:
        ModelFaceStore(const Person::Id &,
                       const PersonInfo &,
                       Database::IDatabase *,
                       const QString& storage);

        std::string name() const override;
        void perform() override;

    private:
        const Person::Id m_pid;
        const PersonInfo m_pi;
        const QString m_storage;
};


class TestSystem: public QObject, public ITaskExecutor::ITask
{
        Q_OBJECT

    public:
        TestSystem(ICoreFactoryAccessor *);
        ~TestSystem();

        std::string name() const override;
        void perform() override;

    private:
        ICoreFactoryAccessor* m_core;

    signals:
        void status(const bool, const QString &) const;
};


class FindBest: public QObject, public ITaskExecutor::ITask
{
        Q_OBJECT

    public:
        FindBest(ICoreFactoryAccessor *, const QStringList &);
        ~FindBest();

        std::string name() const override;
        void perform() override;

    private:
        const QStringList m_faces;
        ICoreFactoryAccessor* m_core;

    signals:
        void best(const QString &) const;
};

#endif // PEOPLEOPERATOR_P_HPP
