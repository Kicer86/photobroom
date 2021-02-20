
#ifndef PROPERTIES_TO_FILTERS_HPP_INCLUDED
#define PROPERTIES_TO_FILTERS_HPP_INCLUDED


#include <QAbstractListModel>

#include "flat_model.hpp"


class PropertiesControlledModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model)
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase)

    public:
        PropertiesControlledModel(QObject* parent = nullptr);
        ~PropertiesControlledModel() = default;

        QAbstractItemModel* model();

        Database::IDatabase* database() const;
        void setDatabase(Database::IDatabase *);

    private:
        FlatModel m_model;
};

#endif // PROPERTIES_TO_FILTERS_HPP_INCLUDED
