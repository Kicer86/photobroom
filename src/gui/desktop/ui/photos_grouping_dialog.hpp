
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>

namespace Ui {
class PhotosGroupingDialog;
}

class PhotosGroupingDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit PhotosGroupingDialog(QWidget *parent = 0);
        ~PhotosGroupingDialog();

    private:
        Ui::PhotosGroupingDialog *ui;
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
