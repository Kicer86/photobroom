
#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/ilogger_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/task_executor_utils.hpp>
#include <QElapsedTimer>
#include <QPromise>

#include "../series_model.hpp"
#include "series_detector.hpp"


using namespace std::placeholders;


SeriesModel::SeriesModel(Database::IDatabase& db, ICoreFactoryAccessor& core)
    : m_logger(core.getLoggerFactory().get("SeriesModel"))
    , m_db(db)
    , m_core(core)
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


QVariant SeriesModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.column() == 0 && index.row() < m_condidates.size())
    {
        const auto& candidate = m_condidates[index.row()];

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
    return m_condidates.size();
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

            SeriesDetector detector(m_db, exif.get(), &promise);

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
    m_condidates = canditates;
    endInsertRows();

    m_loaded = true;
    emit loadedChanged(m_loaded);
}
