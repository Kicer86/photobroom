
#include <core/function_wrappers.hpp>

#include <database/database_qcoro_utils.hpp>
#include <database/photo_data.hpp>
#include "../photo_delta_fetcher.hpp"


PhotoDeltaFetcher::PhotoDeltaFetcher(Database::IDatabase& db)
    : m_db(db)
{
}


void PhotoDeltaFetcher::fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
{
    auto [guard, callback] = make_cancellable([this](const std::vector<Photo::DataDelta>& data)
    {
        emit photoDataDeltaFetched(data);
    });

    m_fetchGuard = std::move(guard);

    if (ids.empty())
        callback(std::vector<Photo::DataDelta>{});
    else
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
            this, std::move(callback),
            "PhotoDeltaFetcher: fetch photos data"
        );
}
