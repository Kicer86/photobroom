
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>
#include "quick_views/selection_manager_component.hpp"


class QAbstractItemModel;
class QItemSelectionModel;


class SelectionToPhotoIdTranslator: public QObject
{
        Q_OBJECT

    public:
        SelectionToPhotoIdTranslator(const SelectionManagerComponent &, const QAbstractItemModel& model, QObject* = nullptr);

    signals:
        void selectionChanged(const std::vector<Photo::Id> &) const;

    private:
        const SelectionManagerComponent & m_selectionManager;
        const QAbstractItemModel& m_model;
        int m_photoIdRole;

        void translate() const;
};

#endif // SELECTIONTOPHOTOIDTRANSLATOR_HPP
