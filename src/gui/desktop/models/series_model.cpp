
#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/itasks_view_utils.hpp>
#include <core/task_executor_utils.hpp>
#include <database/database_tools/series_detector.hpp>
#include <database/database_executor_traits.hpp>
#include <QElapsedTimer>
#include <QPromise>

#include "gui/desktop/utils/groups_manager.hpp"
#include "series_model.hpp"


using namespace std::placeholders;


SeriesModel::SeriesModel(Project& project, ICoreFactoryAccessor& core, ITasksView& taskView)
    : m_logger(core.getLoggerFactory().get("SeriesModel"))
    , m_project(project)
    , m_core(core)
    , m_tasksView(taskView)
    , m_initialized(false)
    , m_loaded(false)
{

}


SeriesModel::~SeriesModel()
{
    m_candidatesFuture.cancel();
    m_candidatesFuture.waitForFinished();
}


bool SeriesModel::isLoaded() const
{
    return m_loaded;
}


void SeriesModel::groupBut(const QSet<int>& excludedRows)
{
    std::vector<std::vector<Photo::Data>> toStore;
    std::vector<GroupCandidate> left;

    for(std::size_t i = 0; i < m_candidates.size(); i++)
    {
        const auto& candidate = m_candidates[i];

        excludedRows.contains(i)?
            left.push_back(candidate):
            toStore.push_back(candidate.members);
    }

    auto& executor = m_core.getTaskExecutor();

    QPromise<void> promise;
    QFuture<void> future = promise.future();;

    runOn(executor, [groups = std::move(toStore), &project = m_project, promise = std::move(promise)]() mutable
    {
        GroupsManager::groupIntoUnified(project, std::move(promise), groups);
    },
    "unified group generation");

    TasksViewUtils::addFutureTask(m_tasksView, future, tr("Saving group details."));

    beginResetModel();
    m_candidates.clear();
    endResetModel();

    updateModel(left);
}


bool SeriesModel::isEmpty() const
{
    return rowCount({}) == 0;
}


QVariant SeriesModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.column() == 0 && index.row() < m_candidates.size())
    {
        const auto& candidate = m_candidates[index.row()];

        if (role == PhotoDataRole)
            return QVariant::fromValue(candidate.members.front());
        else if (role == DetailsRole)
            return QVariant::fromValue(candidate);
        else if (role == GroupTypeRole)
        {
            QString type;
            switch (candidate.type)
            {
                case Group::Type::Invalid:                                              break;
                case Group::Type::Animation: type = tr("Photo series");                 break;
                case Group::Type::HDR:       type = tr("HDR");                          break;
                case Group::Type::Generic:   type = tr("Photos taken at similar time"); break;
                case Group::Type::Collage:   assert(!"not expected nor implemented");   break;
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
    return parent.isValid()? 0: m_candidates.size();
}


bool SeriesModel::canFetchMore(const QModelIndex& parent) const
{
    return parent.isValid() == false && m_initialized == false;
}


void SeriesModel::fetchMore(const QModelIndex& parent)
{
    if (parent.isValid() == false)
    {
        m_initialized = true;

        fetchGroups();
    }
}


QHash<int, QByteArray> SeriesModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();

    roles.insert(
    {
        { DetailsRole,   "details" },
        { PhotoDataRole, "photoData" },
        { GroupTypeRole, "groupType" },
        { MembersRole,   "members" }
    });

    return roles;
}


void SeriesModel::fetchGroups()
{
    auto& executor = m_core.getTaskExecutor();

    m_candidatesFuture = runOn<std::vector<GroupCandidate>>
    (
        executor,
        [this](QPromise<std::vector<GroupCandidate>>& promise)
        {
            IExifReaderFactory& exif = m_core.getExifReaderFactory();

            QElapsedTimer timer;

            auto detectLogger = m_logger->subLogger("SeriesDetector");
            SeriesDetector detector(*detectLogger, m_project.getDatabase(), exif.get(), &promise);

            timer.start();
            promise.addResult(detector.listCandidates());
            m_logger->debug(QString("Photos analysis took %1s").arg(timer.elapsed()/1000.0));
        },
        "SeriesDetector"
    );

    m_candidatesFuture.then(std::bind(&SeriesModel::updateModel, this, _1));
}


void SeriesModel::updateModel(const std::vector<GroupCandidate>& canditates)
{
    beginInsertRows({}, 0, canditates.size() - 1);
    m_candidates = canditates;
    m_logger->info(QString("Got %1 group canditates").arg(canditates.size()));
    endInsertRows();

    m_loaded = true;
    emit loadedChanged(m_loaded);
}
