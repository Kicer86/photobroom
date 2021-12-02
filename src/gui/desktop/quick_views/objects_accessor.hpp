
#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>


class ObjectsAccessor: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase NOTIFY databaseChanged)

    public:
        static ObjectsAccessor& instance();

        void setDatabase(Database::IDatabase *);
        void setCoreFactory(ICoreFactoryAccessor *);

        Database::IDatabase* database() const;
        ICoreFactoryAccessor* coreFactory() const;

    signals:
        void databaseChanged(Database::IDatabase *) const;

    private:
        Database::IDatabase* m_database;
        ICoreFactoryAccessor* m_core;

        ObjectsAccessor(QObject* parent = nullptr);
        ~ObjectsAccessor() = default;
};

#endif