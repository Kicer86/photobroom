
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>

#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/series_candidate.hpp>
#include <project_utils/project.hpp>

class SeriesDetector;

class SeriesModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)

public:
    enum Roles
    {
        DetailsRole = Qt::UserRole + 1,
        PhotoDataRole,
        GroupTypeRole,
        MembersRole,
    };

    SeriesModel(Project &, ICoreFactoryAccessor &);
    ~SeriesModel();

    bool isLoaded() const;
    Q_INVOKABLE void groupBut(const QSet<int> &);
    Q_INVOKABLE bool isEmpty() const;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void loadedChanged(bool) const;

private:
    std::unique_ptr<ILogger> m_logger;
    std::vector<GroupCandidate> m_candidates;
    Project& m_project;
    ICoreFactoryAccessor& m_core;
    QFuture<std::vector<GroupCandidate>> m_candidatesFuture;
    bool m_initialized;
    bool m_loaded;

    void fetchGroups();
    void updateModel(const std::vector<GroupCandidate> &);
};

#endif
