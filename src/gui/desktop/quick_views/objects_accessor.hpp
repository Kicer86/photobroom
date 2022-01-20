
#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <project_utils/project.hpp>

/**
 * @brief A Singleton class providing access to business logic from UI items
 *
 * This class is meant be used in UI only, as it is hard to provide all required objects
 * to UI components via interfaces.
 */
class ObjectsAccessor: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase NOTIFY databaseChanged)
    Q_PROPERTY(Project* project READ project WRITE setProject NOTIFY projectChanged)

    public:
        static ObjectsAccessor& instance();

        void setDatabase(Database::IDatabase *);
        void setProject(Project *);
        void setCoreFactory(ICoreFactoryAccessor *);

        Database::IDatabase* database() const;
        Project* project() const;
        ICoreFactoryAccessor* coreFactory() const;

    signals:
        void databaseChanged(Database::IDatabase *) const;
        void projectChanged(Project *) const;

    private:
        Database::IDatabase* m_database;
        Project* m_project;
        ICoreFactoryAccessor* m_core;

        ObjectsAccessor(QObject* parent = nullptr);
        ~ObjectsAccessor() = default;
};

#endif
