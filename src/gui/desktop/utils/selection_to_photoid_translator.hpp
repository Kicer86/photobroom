
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>
#include <database/photo_data.hpp>
#include "quick_views/selection_manager_component.hpp"


class QAbstractItemModel;
class QItemSelectionModel;


class SelectionToPhotoDataTranslator
{
    public:
        SelectionToPhotoDataTranslator(const SelectionManagerComponent &, const QAbstractItemModel& model);

        std::vector<Photo::Data> getSelectedDatas() const;

    private:
        const SelectionManagerComponent& m_selectionManager;
        const QAbstractItemModel& m_model;
        int m_photoDataRole;
};


class SelectionChangeNotifier: public QObject
{
        Q_OBJECT

    public:
        SelectionChangeNotifier(const SelectionManagerComponent &, const SelectionToPhotoDataTranslator &, QObject* = nullptr);

    signals:
        void selectionChanged(const std::vector<Photo::Data> &) const;

    private:
        const SelectionToPhotoDataTranslator& m_translator;

        void translate() const;
};

#endif // SELECTIONTOPHOTOIDTRANSLATOR_HPP
