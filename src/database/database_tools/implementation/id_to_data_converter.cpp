
#include "../id_to_data_converter.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>


IdToDataConverter::IdToDataConverter(Database::IDatabase& db)
    : m_db(db)
{

}


IdToDataConverter::~IdToDataConverter()
{
    m_callbackCtrl.invalidate();
}


void IdToDataConverter::fetchIds(const std::vector<Photo::Id>& ids)
{
    m_callbackCtrl.invalidate();        // new query, drop any pending tasks

    if (ids.empty())
        storePhotoData(std::vector<Photo::Data>{});
    else
    {
        auto db_task = m_callbackCtrl.make_safe_callback<Database::IBackend&>([ids, this](Database::IBackend& backend)
        {
            std::vector<Photo::Data> data;

            for (const auto& id: ids)
                data.push_back(backend.getPhoto(id));

            storePhotoData(data);
        });

        m_db.exec(db_task);
    }
}


void IdToDataConverter::fetchIds(const std::vector<Photo::Id>& ids, const std::set<Photo::Field>& fields)
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


void IdToDataConverter::storePhotoData(const std::vector<Photo::Data>& data)
{
    emit photoDataFetched(data);
}


void IdToDataConverter::storePhotoData(const std::vector<Photo::DataDelta>& data)
{
    emit photoDataDeltaFetched(data);
}
