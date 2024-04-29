
#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "../photo_delta_fetcher.hpp"


PhotoDeltaFetcher::PhotoDeltaFetcher(Database::IDatabase& db)
    : m_db(db)
{

}


PhotoDeltaFetcher::~PhotoDeltaFetcher()
{
    m_callbackCtrl.invalidate();
}


void PhotoDeltaFetcher::fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
{
    m_callbackCtrl.invalidate();        // new query, drop any pending tasks

    if (ids.empty())
        storePhotoData(std::vector<Photo::DataDelta>{});
    else
    {
        auto db_task = m_callbackCtrl.make_safe_callback<Database::IBackend&>([ids, fields, this](Database::IBackend& backend)
        {
            std::vector<Photo::DataDelta> data;

            for (const auto& id: ids)
                data.push_back(backend.getPhotoDelta(id, fields));

            storePhotoData(data);
        });

        m_db.exec(db_task);
    }
}


void PhotoDeltaFetcher::storePhotoData(const std::vector<Photo::DataDelta>& data)
{
    emit photoDataDeltaFetched(data);
}
