
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(const SelectionManagerComponent& selectionManager,
                                                           const QAbstractItemModel& model)
    : m_selectionManager(selectionManager)
    , m_model(model)
    , m_photoIdRole(-1)
{
    m_photoIdRole = utils::getRoleByName(model, "photoId");

    assert(m_photoIdRole != -1);
}


std::vector<Photo::Id> SelectionToPhotoIdTranslator::getSelectedIds() const
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

    return ids;
}


SelectionChangeNotifier::SelectionChangeNotifier(const SelectionManagerComponent& manager, const QAbstractItemModel& model, QObject* p)
    : QObject(p)
    , m_translator(manager, model)
{
    connect(&manager, &SelectionManagerComponent::selectionChanged,
            this, &SelectionChangeNotifier::translate);
}


void SelectionChangeNotifier::translate() const
{
    const auto ids = m_translator.getSelectedIds();

    emit selectionChanged(ids);
}
