
#include <core/function_wrappers.hpp>
#include <core/ilogger_factory.hpp>
#include <core/iexif_reader.hpp>

#include "../series_detector.hpp"
#include "../series_model.hpp"


using namespace std::placeholders;


SeriesModel::SeriesModel(Database::IDatabase& db, ICoreFactoryAccessor& core)
    : m_db(db)
    , m_core(core)
    , m_initialized(false)
    , m_loaded(false)
{

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
                case Group::Type::Invalid:                           break;
                case Group::Type::Animation: type = tr("Animation"); break;
                case Group::Type::HDR:       type = tr("HDR");       break;
                case Group::Type::Generic:   type = tr("Generic");   break;
            }

            return type;
        }
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

        m_db.exec(std::bind(&SeriesModel::fetchGroups, this, _1));
    }
}


QHash<int, QByteArray> SeriesModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();

    roles.insert(
    {
        { DetailsRole,   "details" },
        { PhotoDataRole, "photoData" },
        { GroupTypeRole, "groupType" }
    });

    return roles;
}


void SeriesModel::fetchGroups(Database::IBackend& backend)
{
    auto logger = m_core.getLoggerFactory().get("SeriesModel");

    IExifReaderFactory& exif = m_core.getExifReaderFactory();
    SeriesDetector detector(backend, exif.get(), *logger.get());

    const auto candidates = detector.listCandidates();

    // go back to main thread
    invokeMethod(this, &SeriesModel::updateModel, candidates);
}


void SeriesModel::updateModel(const std::vector<SeriesDetector::GroupCandidate>& canditates)
{
    beginInsertRows({}, 0, canditates.size() - 1);
    m_condidates = canditates;
    endInsertRows();

    m_loaded = true;
    emit loadedChanged(m_loaded);
}
