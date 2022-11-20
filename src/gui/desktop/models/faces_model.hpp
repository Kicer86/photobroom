
#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QObject>

#include <core/function_wrappers.hpp>
#include <core/icore_factory_accessor.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_manipulator.hpp"


class FacesModel: public QObject
{
        Q_OBJECT

        Q_PROPERTY(Photo::Id photoID MEMBER m_id REQUIRED)
        Q_PROPERTY(Database::IDatabase* database MEMBER m_database REQUIRED)
        Q_PROPERTY(ICoreFactoryAccessor* core MEMBER m_core REQUIRED)

    public:
        FacesModel(QObject* parent = nullptr);

    private:
        Photo::Id m_id;
        Database::IDatabase* m_database = nullptr;
        ICoreFactoryAccessor* m_core = nullptr;
        PeopleManipulator *m_peopleManipulator;
        QVector<QRect> m_faces;
        QString m_photoPath;
        QSize m_photoSize;

        void updateFaceInformation();
        void applyFaceName(const QRect &, const PersonName &);
        void updatePeopleList();
        void selectFace();

        void updateDetectionState(int);

        void apply();

    signals:
        void databaseChanged() const;
        void coreChanged() const;
};

#endif // FACES_DIALOG_HPP
