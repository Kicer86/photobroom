
#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <QObject>

#include <database/idatabase.hpp>

class ObjectsAccessor: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase NOTIFY databaseChanged)

    public:
        ObjectsAccessor(QObject* parent = nullptr);
        ~ObjectsAccessor() = default;

        void setDatabase(Database::IDatabase *);
        Database::IDatabase* database() const;

    signals:
        void databaseChanged(Database::IDatabase *) const;

    private:
        Database::IDatabase* m_database;
};

#endif
