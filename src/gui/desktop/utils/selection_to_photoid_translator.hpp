
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>
#include <database/photo_data.hpp>
#include "quick_views/selection_manager_component.hpp"


class QAbstractItemModel;
class QItemSelectionModel;


class SelectionToPhotoIdTranslator
{
    public:
        SelectionToPhotoIdTranslator(const SelectionManagerComponent &, const QAbstractItemModel& model);

        std::vector<Photo::Id> getSelectedIds() const;
        std::vector<Photo::Data> getSelectedDatas() const;

    private:
        const SelectionManagerComponent& m_selectionManager;
        const QAbstractItemModel& m_model;
        int m_photoIdRole;
        int m_photoDataRole;
};


class SelectionChangeNotifier: public QObject
{
        Q_OBJECT

    public:
        SelectionChangeNotifier(const SelectionManagerComponent &, const QAbstractItemModel& model, QObject* = nullptr);

    signals:
        void selectionChanged(const std::vector<Photo::Data> &) const;

    private:
        SelectionToPhotoIdTranslator m_translator;

        void translate() const;
};

#endif // SELECTIONTOPHOTOIDTRANSLATOR_HPP
