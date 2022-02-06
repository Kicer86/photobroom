
#include "id_to_data_converter.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


IdToDataConverter::IdToDataConverter(Database::IDatabase& db)
    : m_db(db)
{

}


IdToDataConverter::~IdToDataConverter()
{
    m_callbackCtrl.invalidate();
}


void IdToDataConverter::selectedPhotos(const std::vector<Photo::Id>& ids)
{
    m_callbackCtrl.invalidate();        // new selection, drop any pending tasks

    if (ids.empty())
        setSelected({});
    else
    {
        m_selected.lock()->clear();     // new query - drop cached data

        auto db_task = m_callbackCtrl.make_safe_callback<Database::IBackend&>([ids, this](Database::IBackend& backend)
        {
            std::vector<Photo::Data> data;

            for (const auto& id: ids)
                data.push_back(backend.getPhoto(id));

            setSelected(data);
        });

        m_db.exec(db_task);
    }
}


std::vector<Photo::Data> IdToDataConverter::getSelectedDatas() const
{
    return m_selected.lock().get();
}


void IdToDataConverter::setSelected(const std::vector<Photo::Data>& data)
{
    *m_selected.lock() = data;
    emit selectionChanged(data);
}
