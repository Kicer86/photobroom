
#ifndef PROPERTIES_TO_FILTERS_HPP_INCLUDED
#define PROPERTIES_TO_FILTERS_HPP_INCLUDED


#include <QAbstractListModel>

#include "flat_model.hpp"


class PropertiesControlledModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model NOTIFY modelChanged)
    Q_PROPERTY(Database::IDatabase* database READ database WRITE setDatabase)

    Q_PROPERTY(QVariantMap generalFlags READ generalFlags WRITE setGeneralFlags NOTIFY generalFlagsChanged)
    Q_PROPERTY(QVariantMap tags READ tags WRITE setTags NOTIFY tagsChanged)

    public:
        PropertiesControlledModel(QObject* parent = nullptr);
        ~PropertiesControlledModel() = default;

        QAbstractItemModel* model();

        Database::IDatabase* database() const;
        void setDatabase(Database::IDatabase *);

        QVariantMap generalFlags() const;
        QVariantMap tags() const;
        void setGeneralFlags(const QVariantMap &);
        void setTags(const QVariantMap &);

    signals:
        void modelChanged() const;

        void generalFlagsChanged(const QVariantMap &) const;
        void tagsChanged(const QVariantMap &) const;

    private:
        FlatModel m_model;

        QVariantMap m_generalFlags;
        QVariantMap m_tags;

        void updateFilters();
        Database::Filter tagsFilter();
};

#endif // PROPERTIES_TO_FILTERS_HPP_INCLUDED
