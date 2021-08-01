
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoDataTranslator::SelectionToPhotoDataTranslator(Database::IDatabase& db)
    : m_db(db)
{

}

void SelectionToPhotoDataTranslator::selectedPhotos(const std::vector<Photo::Id>& ids)
{
    m_db.exec([ids, this](Database::IBackend& backend)
    {
        std::vector<Photo::Data> data;

        for (const auto& id: ids)
            data.push_back(backend.getPhoto(id));

        setSelected(data);
    });;
}


std::vector<Photo::Data> SelectionToPhotoDataTranslator::getSelectedDatas() const
{
    return m_selected;
}


void SelectionToPhotoDataTranslator::setSelected(const std::vector<Photo::Data>& data)
{
    m_selected = data;
    emit selectionChanged(m_selected);
}
