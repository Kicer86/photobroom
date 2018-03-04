#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

#include <face_recognition/face_recognition.hpp>

struct ICoreFactoryAccessor;
struct IPythonThread;

namespace Ui {
    class FacesDialog;
}

class FacesDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit FacesDialog(ICoreFactoryAccessor *, QWidget *parent = 0);
        ~FacesDialog();

        void load(const QString& photo);

    private:
        FaceRecognition m_faceRecognizer;
        Ui::FacesDialog *ui;
        IPythonThread* m_pythonThread;

        void applyFacesLocations(const QVector<QRect> &);

    signals:
        void gotFacesLocations(const QVector<QRect> &);
};

#endif // FACES_DIALOG_HPP
