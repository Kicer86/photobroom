
#include <database/database_qcoro_utils.hpp>
#include <database/photo_data.hpp>
#include "../photo_delta_fetcher.hpp"


PhotoDeltaFetcher::PhotoDeltaFetcher(Database::IDatabase& db)
    : m_db(db)
{

}


void PhotoDeltaFetcher::fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
{
    Database::coRunOn(
        m_db,
        [ids, fields](Database::IBackend& backend)
        {
            std::vector<Photo::DataDelta> data;
            data.reserve(ids.size());

            for (const auto& id: ids)
                data.push_back(backend.getPhotoDelta(id, fields));

            return data;
        },
        this, &PhotoDeltaFetcher::photoDataDeltaFetched,
        "PhotoDeltaFetcher: fetch photos data"
    );
}
