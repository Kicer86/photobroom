
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(QItemSelectionModel* selectionModel, QObject* p)
    : QObject(p)
    , m_selectionModel(selectionModel)
    , m_propertiesRole(-1)
{
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &SelectionToPhotoIdTranslator::translate);

    QAbstractItemModel* model = selectionModel->model();
    m_propertiesRole = utils::getRoleByName(*model, "photoProperties");

    assert(m_propertiesRole != -1);
}


void SelectionToPhotoIdTranslator::translate() const
{
    const auto indexes = m_selectionModel->selectedIndexes();
    std::vector<Photo::Id> ids;

    for(const QModelIndex& idx: indexes)
    {
        const QVariant dataVariant = idx.data(m_propertiesRole);
        const Photo::Data data = dataVariant.value<Photo::Data>();

        ids.push_back(data.id);
    }

    emit selectionChanged(ids);
}
