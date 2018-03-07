#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

#include <face_recognition/face_recognition.hpp>

class ProjectInfo;
struct ICoreFactoryAccessor;
struct IPythonThread;

namespace Ui {
    class FacesDialog;
}

class FacesDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit FacesDialog(ICoreFactoryAccessor *, const ProjectInfo &, QWidget *parent = 0);
        ~FacesDialog();

        void load(const QString& photo);

        std::vector<std::pair<QRect, QString>> people() const;

    private:
        QVector<QRect> m_faces;
        FaceRecognition m_faceRecognizer;
        QString m_photoPath;
        Ui::FacesDialog *ui;
        IPythonThread* m_pythonThread;

        void applyFacesLocations(const QVector<QRect> &);
        void updateImage();
        void updatePeopleList();

    signals:
        void gotFacesLocations(const QVector<QRect> &);
};

#endif // FACES_DIALOG_HPP
