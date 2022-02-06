
#include "id_to_data_converter.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoDataTranslator::SelectionToPhotoDataTranslator(Database::IDatabase& db)
    : m_db(db)
{

}


SelectionToPhotoDataTranslator::~SelectionToPhotoDataTranslator()
{
    m_callbackCtrl.invalidate();
}


void SelectionToPhotoDataTranslator::selectedPhotos(const std::vector<Photo::Id>& ids)
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


std::vector<Photo::Data> SelectionToPhotoDataTranslator::getSelectedDatas() const
{
    return m_selected.lock().get();
}


void SelectionToPhotoDataTranslator::setSelected(const std::vector<Photo::Data>& data)
{
    *m_selected.lock() = data;
    emit selectionChanged(data);
}
