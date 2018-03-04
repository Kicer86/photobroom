#ifndef FACES_DIALOG_HPP
#define FACES_DIALOG_HPP

#include <QDialog>

namespace Ui {
    class FacesDialog;
}

class FacesDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit FacesDialog(QWidget *parent = 0);
        ~FacesDialog();

    private:
        Ui::FacesDialog *ui;
};

#endif // FACES_DIALOG_HPP
