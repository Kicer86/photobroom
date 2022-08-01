
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>

#include <core/icore_factory_accessor.hpp>
#include <core/itasks_view.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/series_candidate.hpp>
#include <project_utils/project.hpp>


class SeriesDetector;

class SeriesModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(Project* project MEMBER m_project REQUIRED)
    Q_PROPERTY(ICoreFactoryAccessor* coreFactory WRITE setCoreAccessor READ coreAccessor REQUIRED)

public:
    enum Roles
    {
        DetailsRole = Qt::UserRole + 1,
        PhotoDataRole,
        GroupTypeRole,
        MembersRole,
    };

    SeriesModel();
    ~SeriesModel();

    bool isLoaded() const;
    Q_INVOKABLE void group(const QList<int> &);
    Q_INVOKABLE bool isEmpty() const;
    bool isBusy() const;

    void setCoreAccessor(ICoreFactoryAccessor *);
    ICoreFactoryAccessor* coreAccessor() const;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void loadedChanged(bool) const;
    void busyChanged(bool) const;

private:
    std::unique_ptr<ILogger> m_logger;
    std::vector<GroupCandidate> m_candidates;
    Project* m_project = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;
    ITasksView* m_tasksView = nullptr;
    QFuture<std::vector<GroupCandidate>> m_candidatesFuture;
    bool m_initialized;
    bool m_loaded;
    bool m_busy;

    void setBusy(bool);
    void fetchGroups();
    void updateModel(const std::vector<GroupCandidate> &);
    void clear();
};

#endif
