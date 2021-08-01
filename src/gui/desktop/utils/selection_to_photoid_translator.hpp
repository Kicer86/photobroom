
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>
#include <database/photo_data.hpp>
#include <database/idatabase.hpp>
#include "quick_views/selection_manager_component.hpp"


class SelectionToPhotoDataTranslator: public QObject
{
        Q_OBJECT

    public:
        SelectionToPhotoDataTranslator(Database::IDatabase &);

        void selectedPhotos(const std::vector<Photo::Id> &);

        std::vector<Photo::Data> getSelectedDatas() const;

    private:
        std::vector<Photo::Data> m_selected;
        Database::IDatabase& m_db;

        void setSelected(const std::vector<Photo::Data> &);

    signals:
        void selectionChanged(const std::vector<Photo::Data> &) const;
};


#endif
