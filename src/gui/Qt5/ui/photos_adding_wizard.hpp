
#ifndef PHOTOS_ADDING_WIZARD_HPP
#define PHOTOS_ADDING_WIZARD_HPP

#include <QWizard>

class QtExtChooseFile;

namespace Ui
{
    class PhotosAddingWizard;
}

class PhotosAddingWizard : public QWizard
{
    Q_OBJECT

    public:
        explicit PhotosAddingWizard(QWidget *parent = 0);
        PhotosAddingWizard(const PhotosAddingWizard &) = delete;
        ~PhotosAddingWizard();

        PhotosAddingWizard& operator=(const PhotosAddingWizard &) = delete;

    private slots:
        void on_addLocation_clicked();

        void on_photosLocation_textChanged(const QString &arg1);

private:
        Ui::PhotosAddingWizard* ui;
        QtExtChooseFile* m_chooseFile;
};

#endif // PHOTOS_ADDING_WIZARD_HPP
