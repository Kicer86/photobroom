#ifndef PHOTOS_ADDING_WIZARD_HPP
#define PHOTOS_ADDING_WIZARD_HPP

#include <QWizard>

namespace Ui {
class PhotosAddingWizard;
}

class PhotosAddingWizard : public QWizard
{
    Q_OBJECT

public:
    explicit PhotosAddingWizard(QWidget *parent = 0);
    ~PhotosAddingWizard();

private:
    Ui::PhotosAddingWizard *ui;
};

#endif // PHOTOS_ADDING_WIZARD_HPP
