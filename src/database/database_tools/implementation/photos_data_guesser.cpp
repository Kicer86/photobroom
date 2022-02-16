
#include <functional>
#include <QRegularExpression>

#include <core/function_wrappers.hpp>
#include <core/qmodel_utils.hpp>
#include "database/filter.hpp"
#include "database/iphoto_operator.hpp"
#include "../photos_data_guesser.hpp"
#include "data_from_path_extractor.hpp"
#include "photos_analyzer_constants.hpp"


using namespace std::placeholders;

namespace
{
    enum Roles
    {
        photoPathRole            = Qt::UserRole + 1,
        PhotoIDRole,
        SuggestedDateRole,
        SuggestedTimeRole,
    };
}


PhotosDataGuesser::PhotosDataGuesser()
    : m_db(nullptr)
    , m_fetching(false)
    , m_updating(false)
{

}


void PhotosDataGuesser::setDatabase(Database::IDatabase* db)
{
    clear();

    m_db = db;
}


Database::IDatabase * PhotosDataGuesser::database() const
{
    return m_db;
}


bool PhotosDataGuesser::isFetchInProgress() const
{
    return m_fetching;
}


bool PhotosDataGuesser::isUpdateInProgress() const
{
    return m_updating;
}


void PhotosDataGuesser::performAnalysis()
{
    if (m_db != nullptr)
    {
        clear();
        m_db->exec(std::bind(&PhotosDataGuesser::process, this, _1));
        updateFetchStatus(true);
    }
}


void PhotosDataGuesser::applyBut(const QList<int>& excluded)
{
    const std::set excludedSet(excluded.begin(), excluded.end());

    std::vector<CollectedData> photosToProcess;
    photosToProcess.reserve(m_photos.size() - excludedSet.size());

    for (auto i = 0u; i < m_photos.size(); i++)
        if (excludedSet.contains(i) == false)
            photosToProcess.push_back(m_photos[i]);

    updateUpdateStatus(true);
    m_db->exec(std::bind(&PhotosDataGuesser::updatePhotos, this, _1, photosToProcess));
}


Photo::Id PhotosDataGuesser::getId(int row) const
{
    return m_photos[row].photoData.getId();
}


QVariant PhotosDataGuesser::data(const QModelIndex& index, int role) const
{
    const auto& photo = m_photos[index.row()];

    if (role == photoPathRole)
        return photo.photoData.get<Photo::Field::Path>();
    else if (role == PhotoIDRole)
        return QVariant::fromValue(photo.photoData.getId());
    else if (role == SuggestedDateRole)
        return photo.date.toString(Qt::ISODate);
    else if (role == SuggestedTimeRole)
        return photo.time.toString();
    else
        return {};
}


int PhotosDataGuesser::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0 : static_cast<int>(m_photos.size());
}


QHash<int, QByteArray> PhotosDataGuesser::roleNames() const
{
    const auto roles = parseRoles<Roles>();
    const QHash<int, QByteArray> rolesMap(roles.begin(), roles.end());

    return rolesMap;
}


void PhotosDataGuesser::clear()
{
    beginResetModel();
    m_photos.clear();
    endResetModel();
}


void PhotosDataGuesser::updateFetchStatus(bool status)
{
    m_fetching = status;
    emit fetchInProgressChanged(m_fetching);
}


void PhotosDataGuesser::updateUpdateStatus(bool status)
{
    m_updating = status;
    emit updateInProgressChanged(m_updating);
}


void PhotosDataGuesser::process(Database::IBackend& backend)
{
    const Database::FilterPhotosWithFlags analyzed({ { Photo::FlagsE::ExifLoaded, PhotosAnalyzerConsts::ExifFlagVersion } });
    const Database::FilterPhotosWithTag date(Tag::Types::Date);
    const Database::FilterNotMatchingFilter noDate(date);
    const Database::GroupFilter filters = {analyzed, noDate};
    const auto photos = backend.photoOperator().getPhotos(filters);

    invokeMethod(this, &PhotosDataGuesser::photosFetched, photos);
}


void PhotosDataGuesser::processIds(Database::IBackend& backend, const std::vector<Photo::Id>& ids)
{
    static const DataFromPathExtractor extractor;

    std::vector<CollectedData> photos;

    for(const Photo::Id& id: ids)
    {
        const Photo::DataDelta photoData = backend.getPhotoDelta(id, {Photo::Field::Path});
        const auto tags = extractor.extract(photoData.get<Photo::Field::Path>());

        if (tags.empty() == false)
        {
            CollectedData data;

            auto it = tags.find(Tag::Types::Date);

            if (it != tags.end())
                data.date = it->second.getDate();

            it = tags.find(Tag::Types::Time);

            if (it != tags.end())
                data.time = it->second.getTime();

            data.photoData = photoData;
            photos.push_back(data);
        }
    }

    invokeMethod(this, &PhotosDataGuesser::photoDataFetched, photos);
}


void PhotosDataGuesser::updatePhotos(Database::IBackend& backend, const std::vector<CollectedData>& infos)
{
    std::vector<Photo::DataDelta> deltasToStore;
    deltasToStore.reserve(infos.size());

    for (const auto& info: infos)
    {
        auto photoDelta = backend.getPhotoDelta(info.photoData.getId(), { Photo::Field::Tags });
        auto tags = photoDelta.get<Photo::Field::Tags>();

        if (info.date.isValid())
            tags[Tag::Types::Date] = info.date;

        if (info.time.isValid())
            tags[Tag::Types::Time] = info.time;

        photoDelta.insert<Photo::Field::Tags>(tags);

        deltasToStore.push_back(photoDelta);
    }

    if (deltasToStore.empty() == false)
        backend.update(deltasToStore);

    updateUpdateStatus(false);
}


void PhotosDataGuesser::photosFetched(const std::vector<Photo::Id>& ids)
{
    m_db->exec(std::bind(&PhotosDataGuesser::processIds, this, _1, ids));
}


void PhotosDataGuesser::photoDataFetched(const std::vector<CollectedData>& data)
{
    const int count = static_cast<int>(data.size());
    beginInsertRows({}, 0, count - 1);
    m_photos = data;
    endInsertRows();
    updateFetchStatus(false);
}
