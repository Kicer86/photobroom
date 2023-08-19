
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


ENUM_ROLES_SETUP(SeriesModel::Roles);


SeriesModel::SeriesModel()
    : m_logger()
    , m_project()
    , m_core()
{

}


SeriesModel::~SeriesModel()
{
    m_work.request_stop();
}


void SeriesModel::group(const QList<int>& rows)
{
    std::vector<std::vector<GroupCandidate::ExplicitDelta>> toStore;

    for(const int i: rows)
    {
        const auto& candidate = m_candidates[static_cast<std::size_t>(i)];

        toStore.push_back(candidate.members);
    }

    auto& executor = m_core->getTaskExecutor();

    QPromise<void> promise;
    QFuture<void> future = promise.future();
    future.then(std::bind(&SeriesModel::clear, this));

    setState(State::Storing);

    runOn(executor, [groups = std::move(toStore), project = m_project, promise = std::move(promise)]() mutable
    {
        GroupsManager::groupIntoUnified(*project, std::move(promise), groups);
    },
    "unified group generation");

    // TasksViewUtils::addFutureTask(m_tasksView, future, tr("Saving groups details."));
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


bool SeriesModel::isEmpty() const
{
    return rowCount({}) == 0;
}


QVariant SeriesModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.column() == 0 && index.row() < static_cast<int>(m_candidates.size()))
    {
        const auto& candidate = m_candidates[static_cast<std::size_t>(index.row())];

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
            return QVariant::fromValue(candidate.members);
    }

    return {};
}


int SeriesModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0: static_cast<int>(m_candidates.size());
}


QHash<int, QByteArray> SeriesModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    const auto extra = parseRoles<Roles>();
    const QHash<int, QByteArray> extraRoles(extra.begin(), extra.end());
    roles.insert(extraRoles);

    return roles;
}


void SeriesModel::loadData(const std::stop_token& stopToken, StoppableTaskCallback<std::vector<GroupCandidate>> callback)
{
    runOn(
        m_core->getTaskExecutor(),
        [core = m_core, logger = m_logger->subLogger("SeriesDetector"), dbClient = m_project->getDatabase().attach("SeriesDetector"), callback, stopToken]() mutable
        {
            IExifReaderFactory& exif = core->getExifReaderFactory();

            QElapsedTimer timer;

            auto detectLogger = logger->subLogger("SeriesDetector");
            SeriesDetector detector(*detectLogger, dbClient->db(), exif.get(), stopToken);

            timer.start();
            const auto candidates = detector.listCandidates();
            const auto elapsed = timer.elapsed();

            logger->debug(QString("Photos analysis took %1s").arg(static_cast<double>(elapsed)/1000.0));

            callback(candidates);
        },
        "SeriesDetector"
    );
}


void SeriesModel::updateData(const std::vector<GroupCandidate>& canditates)
{
    beginInsertRows({}, 0, static_cast<int>(canditates.size()) - 1);
    m_candidates = canditates;
    m_logger->info(QString("Got %1 group canditates").arg(m_candidates.size()));
    endInsertRows();
}


void SeriesModel::clearData()
{
    beginResetModel();
    m_candidates.clear();
    endResetModel();
}
