
#include <functional>

#include "core/function_wrappers.hpp"
#include "database/filter.hpp"
#include "database/iphoto_operator.hpp"
#include "../photos_data_guesser.hpp"


using namespace std::placeholders;


PhotosDataGuesser::PhotosDataGuesser(Database::IDatabase& db)
    : m_db(db)
{
    m_db.exec(std::bind(&PhotosDataGuesser::proces, this, _1));
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
    std::vector<Photo::DataDelta> photos;

    for(const Photo::Id& id: ids)
    {
        const auto data = backend.getPhotoDelta(id, {Photo::Field::Path});
        photos.push_back(data);
    }
}


void PhotosDataGuesser::photosFetched(const std::vector<Photo::Id>& ids)
{
    m_db.exec(std::bind(&PhotosDataGuesser::procesIds, this, _1, ids));
}
