
#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/itasks_view_utils.hpp>
#include <core/task_executor_utils.hpp>
#include <core/qmodel_utils.hpp>
#include <database/database_tools/series_detector.hpp>
#include <database/database_executor_traits.hpp>
#include <QElapsedTimer>
#include <QPromise>

#include "gui/desktop/utils/groups_manager.hpp"
#include "series_model.hpp"


using namespace std::placeholders;
using namespace Qt::Literals::StringLiterals;


ENUM_ROLES_SETUP(SeriesModel::Roles);


SeriesModel::SeriesModel()
    : m_logger()
    , m_project()
    , m_core()
{

}


SeriesModel::~SeriesModel()
{

}


void SeriesModel::setCoreAccessor(ICoreFactoryAccessor* core)
{
    m_core = core;
    m_logger = m_core->getLoggerFactory().get("SeriesModel");
}


ICoreFactoryAccessor* SeriesModel::coreAccessor() const
{
    return m_core;
}


QVariant SeriesModel::data(const QModelIndex& index, int role) const
{
    const auto& candidates = internalData();

    if (index.isValid() && index.column() == 0 && index.row() < static_cast<int>(candidates.size()))
    {
        const auto& candidate = candidates[static_cast<std::size_t>(index.row())];

        if (role == PhotoDataRole)
            return QVariant::fromValue(candidate.members.front());
        else if (role == DetailsRole)
            return QVariant::fromValue(candidate);
        else if (role == GroupTypeRole)
        {
            QString type;
            switch (candidate.type)
            {
                case GroupCandidate::Type::Series:   type = tr("Photo series");                 break;
                case GroupCandidate::Type::HDR:      type = tr("HDR");                          break;
                case GroupCandidate::Type::Generic:  type = tr("Photos taken at similar time"); break;
            }

            return type;
        }
        else if (role == MembersRole)
        {
            const auto members = candidate.members;
            const std::vector<Photo::DataDelta> members_delta(members.begin(), members.end());

            return QVariant::fromValue(members_delta);
        }
    }

    return {};
}


QHash<int, QByteArray> SeriesModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    const auto extra = parseRoles<Roles>();
    const QHash<int, QByteArray> extraRoles(extra.begin(), extra.end());
    roles.insert(extraRoles);

    return roles;
}


void SeriesModel::loadData(QPromise<DataVector>&& promise)
{
    runOn(
        m_core->getTaskExecutor(),
        [core = m_core, logger = m_logger->subLogger("SeriesDetector"), dbClient = m_project->getDatabase().attach(u"SeriesDetector"_s), promise = std::move(promise)]() mutable
        {
            IExifReaderFactory& exif = core->getExifReaderFactory();

            QElapsedTimer timer;

            auto detectLogger = logger->subLogger("SeriesDetector");
            SeriesDetector detector(*detectLogger, dbClient->db(), exif.get(), promise);

            timer.start();
            const auto candidates = detector.listCandidates();
            const auto elapsed = timer.elapsed();

            logger->debug(QString("Photos analysis took %1s").arg(static_cast<double>(elapsed)/1000.0));
            logger->info(QString("Got %1 group canditates").arg(candidates.size()));

            promise.addResult(candidates);
            promise.finish();
        },
        "SeriesDetector"
    );
}


void SeriesModel::applyRows(const QList<int>& rows, AHeavyListModel::ApplyToken token)
{
    std::vector<std::vector<GroupCandidate::ExplicitDelta>> toStore;

    const auto& candidates = internalData();

    for(const int i: rows)
    {
        const auto& candidate = candidates[static_cast<std::size_t>(i)];

        toStore.push_back(candidate.members);
    }

    auto& executor = m_core->getTaskExecutor();

    runOn(executor, [groups = std::move(toStore), project = m_project, token = std::move(token)]() mutable
    {
        QPromise<void> promise;
        GroupsManager::groupIntoUnified(*project, std::move(promise), groups);
    },
    "unified group generation");

    // TasksViewUtils::addFutureTask(m_tasksView, future, tr("Saving groups details."));
}
