
#ifndef SERIESMODEL_HPP
#define SERIESMODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>

#include <core/icore_factory_accessor.hpp>
#include <core/itasks_view.hpp>
#include <database/idatabase.hpp>
#include <database/database_tools/series_candidate.hpp>
#include <project_utils/project.hpp>

#include "aheavy_list_model.hpp"


class SeriesDetector;

class SeriesModel: public AHeavyListModel<GroupCandidate>
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

    void setCoreAccessor(ICoreFactoryAccessor *);
    ICoreFactoryAccessor* coreAccessor() const;

    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::unique_ptr<ILogger> m_logger;
    Project* m_project = nullptr;
    ICoreFactoryAccessor* m_core = nullptr;

    void loadData(QPromise<DataVector> &&) override;
    void applyRows(const QList<int> & , AHeavyListModel::ApplyToken ) override;
};

#endif
