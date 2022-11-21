
#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QAbstractListModel>

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_manipulator.hpp"


class FacesModel: public QAbstractListModel
{
        Q_OBJECT

        Q_PROPERTY(Photo::Id photoID MEMBER m_id REQUIRED)
        Q_PROPERTY(Database::IDatabase* database MEMBER m_database REQUIRED)
        Q_PROPERTY(ICoreFactoryAccessor* core MEMBER m_core REQUIRED)
        Q_PROPERTY(int state READ state NOTIFY stateChanged)

    public:
        FacesModel(QObject* parent = nullptr);

        int state() const;

        // QAbstractItemModel:
        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;

    private:
        Photo::Id m_id;
        Database::IDatabase* m_database = nullptr;
        ICoreFactoryAccessor* m_core = nullptr;
        std::unique_ptr<PeopleManipulator> m_peopleManipulator;
        QVector<QRect> m_faces;
        QString m_photoPath;
        QSize m_photoSize;
        int m_state = 0;

        void updateFaceInformation();
        void applyFaceName(const QRect &, const PersonName &);
        void updatePeopleList();
        void selectFace();

        void initialSetup();
        void updateDetectionState(int);

        void apply();

    signals:
        void stateChanged(int);
};

#endif // FACES_DIALOG_HPP
