
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include <database/photo_data.hpp>
#include "utils/model_index_utils.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(const QItemSelectionModel* selectionModel, QObject* p)
    : QObject(p)
    , m_selectionModel(selectionModel)
    , m_photoIdRole(-1)
{
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &SelectionToPhotoIdTranslator::translate);

    const QAbstractItemModel* model = selectionModel->model();
    m_photoIdRole = utils::getRoleByName(*model, "photoId");

    assert(m_photoIdRole != -1);
}


void SelectionToPhotoIdTranslator::translate() const
{
    const auto indexes = m_selectionModel->selectedIndexes();
    std::vector<Photo::Id> ids;

    for(const QModelIndex& idx: indexes)
    {
        const QVariant dataVariant = idx.data(m_photoIdRole);
        const Photo::Id id(dataVariant.toInt());

        assert(id.valid());
        ids.push_back(id);
    }

    emit selectionChanged(ids);
}
