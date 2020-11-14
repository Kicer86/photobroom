#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

#include <core/function_wrappers.hpp>
#include <database/idatabase.hpp>
#include <database/person_data.hpp>
#include <face_recognition/face_recognition.hpp>

#include "utils/people_manipulator.hpp"


class QTableWidgetItem;

struct ICoreFactoryAccessor;
struct IExifReader;

namespace Ui {
    class FacesDialog;
}

class Project;
struct ICompleterFactory;

class FacesDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit FacesDialog(const Photo::Data &, ICompleterFactory *, ICoreFactoryAccessor *, Project *, QWidget *parent = 0);
        ~FacesDialog();

    private:
        const Photo::Id m_id;
        PeopleManipulator m_peopleManipulator;
        QVector<QRect> m_faces;
        QString m_photoPath;
        Ui::FacesDialog *ui;
        IExifReader* m_exif;

        void updateFaceInformation();
        void applyFaceName(const QRect &, const PersonName &);
        void applyUnassigned(const Photo::Id &, const QStringList &);
        void updateImage();
        void updatePeopleList();

        void updateDetectionState(int);

        void setUnassignedVisible(bool);
        void apply();
};

#endif // FACES_DIALOG_HPP
