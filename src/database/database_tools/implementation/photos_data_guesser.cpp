
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
    };
}


PhotosDataGuesser::PhotosDataGuesser()
    : m_db(nullptr)
{

}


void PhotosDataGuesser::setDatabase(Database::IDatabase* db)
{
    m_db = db;

    if (m_db != nullptr)
        m_db->exec(std::bind(&PhotosDataGuesser::proces, this, _1));
}


Database::IDatabase * PhotosDataGuesser::database() const
{
    return m_db;
}


QVariant PhotosDataGuesser::data(const QModelIndex& index, int role) const
{
    if (role == Path)
        return m_photos[index.row()].photoData.get<Photo::Field::Path>();
    else if (role == SuggestedDate)
        return m_photos[index.row()].date;
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
        { SuggestedDate , "suggestedDate" }
    };
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
    std::vector<CollectedData> photos;
    const QRegularExpression dateExpression("^[^0-9]*([0-9]{4})-?([0-9]{2})-?([0-9]{2})[^0-9]*$");

    for(const Photo::Id& id: ids)
    {
        CollectedData data;

        data.photoData = backend.getPhotoDelta(id, {Photo::Field::Path});

        const auto match = dateExpression.match(data.photoData.get<Photo::Field::Path>());
        if (match.hasMatch())
        {
            const QStringList captured = match.capturedTexts();
            data.date = QString("%1-%2-%3")
                            .arg(captured[1])
                            .arg(captured[2])
                            .arg(captured[3]);
        }

        photos.push_back(data);
    }

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
}
