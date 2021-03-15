
#include <functional>

#include "core/function_wrappers.hpp"
#include "database/filter.hpp"
#include "database/iphoto_operator.hpp"
#include "../photos_data_completer.hpp"


using namespace std::placeholders;


PhotosDataCompleter::PhotosDataCompleter(Database::IDatabase& db)
    : m_db(db)
{
    m_db.exec(std::bind(&PhotosDataCompleter::proces, this, _1));
}


void PhotosDataCompleter::proces(Database::IBackend& backend)
{
    const Database::FilterPhotosWithFlags analyzed({ { Photo::FlagsE::ExifLoaded, 1 } });
    const Database::FilterPhotosWithTag noDate(TagTypes::Date, TagValue(), Database::FilterPhotosWithTag::ValueMode::Equal, true);
    const Database::GroupFilter filters = {analyzed, noDate};
    const auto photos = backend.photoOperator().getPhotos(filters);

    invokeMethod(this, &PhotosDataCompleter::photosFetched, photos);
}


void PhotosDataCompleter::photosFetched(const std::vector<Photo::Id>& ids)
{
}
