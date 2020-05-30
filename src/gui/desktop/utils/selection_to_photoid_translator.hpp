
#ifndef SELECTIONTOPHOTOIDTRANSLATOR_HPP
#define SELECTIONTOPHOTOIDTRANSLATOR_HPP

#include <QObject>

#include <database/photo_types.hpp>

class QAbstractItemModel;
class QItemSelectionModel;


class SelectionToPhotoIdTranslator: public QObject
{
        Q_OBJECT

    public:
        SelectionToPhotoIdTranslator(const QItemSelectionModel *, QObject* = nullptr);

    signals:
        void selectionChanged(const std::vector<Photo::Id> &) const;

    private:
        const QItemSelectionModel* m_selectionModel;
        int m_photoIdRole;

        void translate() const;
};

#endif // SELECTIONTOPHOTOIDTRANSLATOR_HPP
