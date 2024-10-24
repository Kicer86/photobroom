
#include <QItemSelectionModel>
#include <QPromise>

#include <database/photo_data.hpp>
#include "../photo_delta_fetcher.hpp"


PhotoDeltaFetcher::PhotoDeltaFetcher(Database::IDatabase& db)
    : m_db(db)
{

}


PhotoDeltaFetcher::~PhotoDeltaFetcher()
{
    m_dataFetchFuture.cancel();
    m_dataFetchFuture.waitForFinished();
}


void PhotoDeltaFetcher::fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
{
    m_dataFetchFuture.cancel();        // new query, drop any pending tasks

    if (ids.empty())
        storePhotoData(std::vector<Photo::DataDelta>{});
    else
    {
        QPromise<std::vector<Photo::DataDelta>> promise;
        m_dataFetchFuture = promise.future();
        m_dataFetchFuture.then([this](const std::vector<Photo::DataDelta>& data)
        {
            storePhotoData(data);
        });

        auto db_task = [ids, fields, promise = std::move(promise)](Database::IBackend& backend) mutable
        {
            std::vector<Photo::DataDelta> data;

            for (const auto& id: ids)
                data.push_back(backend.getPhotoDelta(id, fields));

            promise.addResult(data);
            promise.finish();
        };

        m_db.exec(std::move(db_task));
    }
}


void PhotoDeltaFetcher::storePhotoData(const std::vector<Photo::DataDelta>& data)
{
    emit photoDataDeltaFetched(data);
}
