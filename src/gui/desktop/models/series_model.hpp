
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>
#include <stop_token>

#include <core/icore_factory_accessor.hpp>
#include <core/itasks_view.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/series_candidate.hpp>
#include <project_utils/project.hpp>

#include "aheavy_list_model.hpp"


class SeriesDetector;

class SeriesModel: public AHeavyListModel<std::vector<GroupCandidate>>
{
    Q_OBJECT
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

    Q_INVOKABLE void group(const QList<int> &);
    Q_INVOKABLE bool isEmpty() const;

    void setCoreAccessor(ICoreFactoryAccessor *);
    ICoreFactoryAccessor* coreAccessor() const;

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::unique_ptr<ILogger> m_logger;
    std::vector<GroupCandidate> m_candidates;
    Project* m_project = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;
    std::stop_source m_work;

    void loadData(const std::stop_token& stopToken, StoppableTaskCallback<std::vector<GroupCandidate>>) override;
    void updateData(const std::vector<GroupCandidate> &) override;
    void clearData() override;
};

#endif
