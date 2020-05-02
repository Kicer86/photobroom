
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include "models/photo_properties.hpp"
#include "utils/model_index_utils.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(QItemSelectionModel* selectionModel, QObject* p)
    : QObject(p)
    , m_selectionModel(selectionModel)
    , m_propertiesRole(-1)
{
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &SelectionToPhotoIdTranslator::translate);

    QAbstractItemModel* model = selectionModel->model();
    m_propertiesRole = utils::getRoleByName(model, "photoProperties");

    assert(m_propertiesRole != -1);
}


void SelectionToPhotoIdTranslator::translate() const
{
    const auto indexes = m_selectionModel->selectedIndexes();
    std::vector<Photo::Id> ids;

    for(const QModelIndex& idx: indexes)
    {
        const QVariant rawProperties = idx.data(m_propertiesRole);
        const PhotoProperties properties = rawProperties.value<PhotoProperties>();

        ids.push_back(properties.m_id);
    }

    emit selectionChanged(ids);
}
