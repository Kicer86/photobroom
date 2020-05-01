
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>

class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;


class SelectionToPhotoIdTranslator: public QObject
{
        Q_OBJECT

    public:
        SelectionToPhotoIdTranslator(QItemSelectionModel *, QObject* = nullptr);

    signals:
        void selectionChanged(const std::vector<Photo::Id> &) const;

    private:
        QAbstractItemModel* m_model;
        int m_propertiesRole;

        void translate(const QItemSelection &) const;
};

#endif // SELECTIONTOPHOTOIDTRANSLATOR_HPP
