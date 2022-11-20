
#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QQmlEngine>

#include <core/function_wrappers.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_manipulator.hpp"



struct ICoreFactoryAccessor;
class Project;

class FacesModel: public QObject
{
        Q_OBJECT
        QML_ELEMENT

    public:
        FacesModel(QObject* parent = 0 );
        ~FacesModel();

    private:
        const Photo::Id m_id;
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
};

#endif // FACES_DIALOG_HPP
