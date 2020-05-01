
#include "selection_to_photoid_translator.hpp"

#include <QItemSelectionModel>

#include "models/photo_properties.hpp"


SelectionToPhotoIdTranslator::SelectionToPhotoIdTranslator(QItemSelectionModel* selectionModel, QObject* p)
    : QObject(p)
    , m_model(selectionModel->model())
    , m_propertiesRole(-1)
{
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &SelectionToPhotoIdTranslator::translate);

    const auto roles = m_model->roleNames();

    // Find photo properties role. Model needs to provide it
    const auto it = std::find_if(roles.begin(), roles.end(), [](const auto roleName){
        return roleName == "photoProperties";
    });

    assert(it != roles.end());

    m_propertiesRole = it.key();
}


void SelectionToPhotoIdTranslator::translate(const QItemSelection& selection) const
{
    const auto indexes = selection.indexes();
    std::vector<Photo::Id> ids;

    for(const QModelIndex& idx: indexes)
    {
        const QVariant rawProperties = idx.data(m_propertiesRole);
        const PhotoProperties properties = rawProperties.value<PhotoProperties>();

        ids.push_back(properties.m_id);
    }

    emit selectionChanged(ids);
}
