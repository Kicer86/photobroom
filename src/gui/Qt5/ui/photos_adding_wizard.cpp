
#include "photos_adding_wizard.hpp"

#include "ui_photos_adding_wizard.h"

#include <QFileDialog>

#include <OpenLibrary/QtExt/qtext_choosefile.hpp>

namespace
{
    struct OpenDir: QtExtChooseFileDialog
    {
        OpenDir(): m_result() {}
        OpenDir(const OpenDir &) = delete;

        OpenDir& operator=(const OpenDir &) = delete;

        virtual int exec()
        {
            m_result = QFileDialog::getExistingDirectory(nullptr, tr("Choose directory with photos"));

            return m_result.isEmpty() == false? 1 : 0;
        }

        virtual QString result() const
        {
            return m_result;
        }

        QString m_result;
    };
}


PhotosAddingWizard::PhotosAddingWizard(QWidget *_parent) :
    QWizard(_parent),
    ui(new Ui::PhotosAddingWizard),
    m_chooseFile(nullptr)
{
    ui->setupUi(this);

    m_chooseFile = new QtExtChooseFile(ui->photosLocationBrowse, ui->photosLocation, new OpenDir);
}


PhotosAddingWizard::~PhotosAddingWizard()
{
    delete ui;
}


void PhotosAddingWizard::on_addLocation_clicked()
{

}


void PhotosAddingWizard::on_photosLocation_textChanged(const QString &text)
{
    ui->addLocation->setDisabled(text.isEmpty());
}
