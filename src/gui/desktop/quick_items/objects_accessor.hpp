
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

    Q_PROPERTY(Database::IDatabase* database READ database NOTIFY databaseChanged)
    Q_PROPERTY(Project* project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(ICoreFactoryAccessor* coreFactory READ coreFactory WRITE setCoreFactory NOTIFY coreFactoryChanged)
    Q_PROPERTY(QStringList recentProjects READ recentProjects WRITE setRecentProjects NOTIFY recentProjectsChanged)
    Q_PROPERTY(bool projectOpen READ projectOpen NOTIFY projectOpenChanged)

    public:
        static ObjectsAccessor& instance();

        void setProject(Project *);
        void setCoreFactory(ICoreFactoryAccessor *);
        void setRecentProjects(const QStringList &);

        Database::IDatabase* database() const;
        Project* project() const;
        ICoreFactoryAccessor* coreFactory() const;
        const QStringList& recentProjects() const;
        bool projectOpen() const;

    signals:
        void databaseChanged(Database::IDatabase *) const;
        void projectChanged(Project *) const;
        void coreFactoryChanged(ICoreFactoryAccessor *) const;
        void recentProjectsChanged(const QStringList &) const;
        void projectOpenChanged(bool) const;

    private:
        QStringList m_recentProjects;
        Project* m_project;
        ICoreFactoryAccessor* m_core;

        ObjectsAccessor(QObject* parent = nullptr);
        ~ObjectsAccessor() = default;
};

#endif
