#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

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
        Ui::FacesDialog *ui;
        IPythonThread* m_pythonThread;
};

#endif // FACES_DIALOG_HPP
