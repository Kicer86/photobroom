
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
        SuggestedDate,
        SuggestedTime,
    };
}


PhotosDataGuesser::PhotosDataGuesser()
    : m_db(nullptr)
    , m_fetching(false)
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


void PhotosDataGuesser::performAnalysis()
{
    if (m_db != nullptr)
    {
        clear();
        m_db->exec(std::bind(&PhotosDataGuesser::proces, this, _1));
        updateFetchStatus(true);
    }
}


QVariant PhotosDataGuesser::data(const QModelIndex& index, int role) const
{
    if (role == Path)
        return m_photos[index.row()].photoData.get<Photo::Field::Path>();
    else if (role == SuggestedDate)
        return m_photos[index.row()].date.toString(Qt::ISODate);
    else if (role == SuggestedTime)
        return m_photos[index.row()].time.toString();
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


void PhotosDataGuesser::proces(Database::IBackend& backend)
{
    const Database::FilterPhotosWithFlags analyzed({ { Photo::FlagsE::ExifLoaded, 1 } });
    const Database::FilterPhotosWithTag noDate(TagTypes::Date, TagValue(), Database::FilterPhotosWithTag::ValueMode::Equal, true);
    const Database::GroupFilter filters = {analyzed, noDate};
    const auto photos = backend.photoOperator().getPhotos(filters);

    invokeMethod(this, &PhotosDataGuesser::photosFetched, photos);
}


void PhotosDataGuesser::procesIds(Database::IBackend& backend, const std::vector<Photo::Id>& ids)
{
    //                                       <  NOT NUM  ><  YEAR  >     <  MONTH >     <  DAY   >[  _ <  HOUR  >  < MINUTE >  < SECOND >] < NOT NUM |E>
    const QRegularExpression dateExpression("(?:[^0-9]+|^)([0-9]{4})[.-]?([0-9]{2})[.-]?([0-9]{2})(?:_?([0-9]{2}).?([0-9]{2}).?([0-9]{2}))?(?:[^0-9]+|$)");

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
        }

        photos.push_back(data);
    }

    std::partition(photos.begin(), photos.end(), [](const auto& data) { return data.date.isValid(); });

    invokeMethod(this, &PhotosDataGuesser::photoDataFetched, photos);
}


void PhotosDataGuesser::photosFetched(const std::vector<Photo::Id>& ids)
{
    m_db->exec(std::bind(&PhotosDataGuesser::procesIds, this, _1, ids));
}


void PhotosDataGuesser::photoDataFetched(const std::vector<CollectedData>& data)
{
    const auto count = data.size();
    beginInsertRows({}, 0, count - 1);
    m_photos = data;
    endInsertRows();
    updateFetchStatus(false);
}
