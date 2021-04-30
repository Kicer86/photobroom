
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/series_detector.hpp>
#include "database_export.h"


class DATABASE_EXPORT SeriesModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)

public:
    enum Roles
    {
        DetailsRole = Qt::UserRole + 1,
        PhotoDataRole,
        GroupTypeRole,
    };

    SeriesModel(Database::IDatabase &, ICoreFactoryAccessor &);

    bool isLoaded() const;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void loadedChanged(bool) const;

private:
    std::vector<SeriesDetector::GroupCandidate> m_condidates;
    Database::IDatabase& m_db;
    ICoreFactoryAccessor& m_core;
    bool m_initialized;
    bool m_loaded;

    void fetchGroups(Database::IBackend &);
    void updateModel(const std::vector<SeriesDetector::GroupCandidate> &);
};

#endif
