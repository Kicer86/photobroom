#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

#include <core/callback_ptr.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_operator.hpp"


struct ICoreFactoryAccessor;
struct IPythonThread;

namespace Ui {
    class FacesDialog;
}

class Project;

class FacesDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit FacesDialog(const Photo::Data &, ICoreFactoryAccessor *, Project *, QWidget *parent = 0);
        ~FacesDialog();

        std::vector<std::pair<FaceData, QString>> faces() const;

    private:
        PeopleOperator m_people;
        safe_callback_ctrl m_safeCallback;
        QVector<FaceData> m_faces;
        QString m_photoPath;
        Ui::FacesDialog *ui;
        IPythonThread* m_pythonThread;
        int m_facesToAnalyze;

        void applyFacesLocations(const Photo::Id &, const QVector<FaceData> &);
        void applyFaceName(const Photo::Id &, const FaceData &, const PersonData &);
        void applyUnassigned(const Photo::Id &, const QStringList &);
        void updateImage();
        void updatePeopleList();

        void setUnassignedVisible(bool);
};

#endif // FACES_DIALOG_HPP
