
#include <functional>
#include <QRegularExpression>

#include "core/function_wrappers.hpp"
#include "database/filter.hpp"
#include "database/iphoto_operator.hpp"
#include "../photos_data_guesser.hpp"


using namespace std::placeholders;

namespace
{
    enum Roles
    {
        Path            = Qt::UserRole + 1,
        PhotoID,
        SuggestedDate,
        SuggestedTime,
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


QVariant PhotosDataGuesser::data(const QModelIndex& index, int role) const
{
    const auto& photo = m_photos[index.row()];

    if (role == Path)
        return photo.photoData.get<Photo::Field::Path>();
    else if (role == PhotoID)
        return QVariant::fromValue(photo.photoData.getId());
    else if (role == SuggestedDate)
        return photo.date.toString(Qt::ISODate);
    else if (role == SuggestedTime)
        return photo.time.toString();
    else
        return {};
}


int PhotosDataGuesser::rowCount(const QModelIndex& parent) const
{
    return parent.isValid()? 0 : m_photos.size();
}


QHash<int, QByteArray> PhotosDataGuesser::roleNames() const
{
    return {
        { Path,           "photoPath" },
        { PhotoID,        "photoID" },
        { SuggestedDate , "suggestedDate" },
        { SuggestedTime , "suggestedTime" },
    };
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
    const Database::FilterPhotosWithFlags analyzed({ { Photo::FlagsE::ExifLoaded, 1 } });
    const Database::FilterPhotosWithTag date(TagTypes::Date);
    const Database::FilterNotMatchingFilter noDate(date);
    const Database::GroupFilter filters = {analyzed, noDate};
    const auto photos = backend.photoOperator().getPhotos(filters);

    invokeMethod(this, &PhotosDataGuesser::photosFetched, photos);
}


void PhotosDataGuesser::processIds(Database::IBackend& backend, const std::vector<Photo::Id>& ids)
{
    //                                       <  NOT NUM  ><  YEAR  >     <  MONTH >     <  DAY   >[   _   <  HOUR  >     < MINUTE >     < SECOND >] < NOT NUM |E>
    const QRegularExpression dateExpression("(?:[^0-9]+|^)([0-9]{4})[.-]?([0-9]{2})[.-]?([0-9]{2})(?:[_ ]?([0-9]{2})[.;]?([0-9]{2})[.;]?([0-9]{2}))?(?:[^0-9]+|$)");

    std::vector<CollectedData> photos;

    for(const Photo::Id& id: ids)
    {
        CollectedData data;

        data.photoData = backend.getPhotoDelta(id, {Photo::Field::Path});

        const auto match = dateExpression.match(data.photoData.get<Photo::Field::Path>());
        if (match.hasMatch())
        {
            const QStringList captured = match.capturedTexts();

            const int y = captured[1].toInt();
            const int m = captured[2].toInt();
            const int d = captured[3].toInt();
            const QDate date(y, m, d);

            if (date.isValid())
            {
                data.date = date;

                if (captured.size() == 7)
                {
                    const int hh = captured[4].toInt();
                    const int mm = captured[5].toInt();
                    const int ss = captured[6].toInt();
                    const QTime time(hh, mm, ss);

                    if (time.isValid())
                        data.time = time;
                }

                photos.push_back(data);
            }
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
            tags[TagTypes::Date] = info.date;

        if (info.time.isValid())
            tags[TagTypes::Time] = info.time;

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
    const auto count = data.size();
    beginInsertRows({}, 0, count - 1);
    m_photos = data;
    endInsertRows();
    updateFetchStatus(false);
}
