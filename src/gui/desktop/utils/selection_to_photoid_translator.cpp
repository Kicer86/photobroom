
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoDataTranslator::SelectionToPhotoDataTranslator(const SelectionManagerComponent& selectionManager,
                                                             const QAbstractItemModel& model)
    : m_selectionManager(selectionManager)
    , m_model(model)
    , m_photoDataRole(-1)
{
    m_photoDataRole = utils::getRoleByName(model, "photoData");

    assert(m_photoDataRole != -1);
}


std::vector<Photo::Data> SelectionToPhotoDataTranslator::getSelectedDatas() const
{
    const auto rows = m_selectionManager.selected();
    std::vector<Photo::Data> datas;

    for(const int& row: rows)
    {
        const QModelIndex idx = m_model.index(row, 0);
        const QVariant dataVariant = idx.data(m_photoDataRole);
        const Photo::Data data(dataVariant.value<Photo::Data>());

        datas.push_back(data);
    }

    return datas;
}


SelectionChangeNotifier::SelectionChangeNotifier(const SelectionManagerComponent& manager, const SelectionToPhotoDataTranslator& translator, QObject* p)
    : QObject(p)
    , m_translator(translator)
{
    connect(&manager, &SelectionManagerComponent::selectionChanged,
            this, &SelectionChangeNotifier::translate);
}


void SelectionChangeNotifier::translate() const
{
    const auto datas = m_translator.getSelectedDatas();

    emit selectionChanged(datas);
}
