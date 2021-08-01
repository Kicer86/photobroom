
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <core/function_wrappers.hpp>
#include <database/photo_types.hpp>
#include <database/photo_data.hpp>
#include <database/idatabase.hpp>
#include "quick_views/selection_manager_component.hpp"


class SelectionToPhotoDataTranslator: public QObject
{
        Q_OBJECT

    public:
        SelectionToPhotoDataTranslator(Database::IDatabase &);
        ~SelectionToPhotoDataTranslator();

        void selectedPhotos(const std::vector<Photo::Id> &);

        std::vector<Photo::Data> getSelectedDatas() const;

    private:
        safe_callback_ctrl m_callbackCtrl;
        std::vector<Photo::Data> m_selected;
        Database::IDatabase& m_db;

        void setSelected(const std::vector<Photo::Data> &);

    signals:
        void selectionChanged(const std::vector<Photo::Data> &) const;
};


#endif
