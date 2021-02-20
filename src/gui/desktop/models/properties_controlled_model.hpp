
#ifndef PROPERTIES_TO_FILTERS_HPP_INCLUDED
#define PROPERTIES_TO_FILTERS_HPP_INCLUDED


#include <QAbstractListModel>

#include "flat_model.hpp"


class PropertiesControlledModel: public QAbstractListModel
{
    Q_OBJECT
    // Q_PROPERTY(APhotoInfoModel model READ model WRITE setModel)
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase)

    public:
        PropertiesControlledModel(QObject* parent = nullptr);
        ~PropertiesControlledModel() = default;

        QVariant data(const QModelIndex & index, int role) const override;
        int rowCount(const QModelIndex & parent) const override;

        Database::IDatabase* database() const;
        void setDatabase(Database::IDatabase *);

    private:
        FlatModel m_model;
};

#endif // PROPERTIES_TO_FILTERS_HPP_INCLUDED
