
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(const SelectionManagerComponent& selectionManager,
                                                           const QAbstractItemModel& model,
                                                           QObject* p)
    : QObject(p)
    , m_selectionManager(selectionManager)
    , m_model(model)
    , m_photoIdRole(-1)
{
    connect(&m_selectionManager, &SelectionManagerComponent::selectionChanged,
            this, &SelectionToPhotoIdTranslator::translate);

    m_photoIdRole = utils::getRoleByName(model, "photoId");

    assert(m_photoIdRole != -1);
}


void SelectionToPhotoIdTranslator::translate() const
{
    const auto rows = m_selectionManager.selected();
    std::vector<Photo::Id> ids;

    for(const int& row: rows)
    {
        const QModelIndex idx = m_model.index(row, 0);
        const QVariant dataVariant = idx.data(m_photoIdRole);
        const Photo::Id id(dataVariant.toInt());

        assert(id.valid());
        ids.push_back(id);
    }

    emit selectionChanged(ids);
}
