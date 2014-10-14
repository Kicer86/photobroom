
#include "photos_adding_wizard.hpp"


#include <QFileDialog>

#include <OpenLibrary/QtExt/qtext_choosefile.hpp>

#include <analyzer/photo_crawler_builder.hpp>

#include "components/staged_photos_data_model.hpp"
#include "ui_photos_adding_wizard.h"

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


PhotosReceiver::PhotosReceiver(): m_model(nullptr)
{

}


void PhotosReceiver::setModel(StagedPhotosDataModel *model)
{
    m_model = model;
}


void PhotosReceiver::found(const QString &path)
{
    m_model->addPhoto(path);
}



PhotosAddingWizard::PhotosAddingWizard(QWidget *_parent) :
    QWizard(_parent),
    ui(new Ui::PhotosAddingWizard),
    m_chooseFile(nullptr),
    m_mediaNotification(nullptr)
{
    ui->setupUi(this);

    m_chooseFile.reset(new QtExtChooseFile(ui->photosLocationBrowse, ui->photosLocation, new OpenDir));

    PhotosReceiver* photosReceiver = new PhotosReceiver;
    photosReceiver->setModel(ui->photosView->model());
    m_mediaNotification.reset(photosReceiver);

    connect(photosReceiver, SIGNAL(finished()), this, SLOT(crawlingDone()));
}


PhotosAddingWizard::~PhotosAddingWizard()
{
    delete ui;
}


void PhotosAddingWizard::set(Database::IDatabase* db)
{
    ui->photosView->model()->setDatabase(db);
}


void PhotosAddingWizard::set(IConfiguration* configuration)
{
    ui->photosView->set(configuration);
}


void PhotosAddingWizard::on_photosLocation_textChanged(const QString &text)
{
    QAbstractButton* nextButton = button(QWizard::NextButton);
    const bool disabled = text.isEmpty();
    nextButton->setDisabled(disabled);
}


void PhotosAddingWizard::on_PhotosAddingWizard_currentIdChanged(int id)
{
    //on first page, block next buton until any path is provided
    if (id == 0)
        on_photosLocation_textChanged(ui->photosLocation->text());
    else if (id == 1)
    {
        //run crawler
        const QString path = ui->photosLocation->text();

        IPhotoCrawler* crawler = PhotoCrawlerBuilder().build();
        crawler->crawl(path, m_mediaNotification.get());

        //lock finish button
        QAbstractButton* finishButton = button(QWizard::FinishButton);
        finishButton->setDisabled(true);
    }
}


void PhotosAddingWizard::crawlingDone()
{
    //unlock finish button
    QAbstractButton* finishButton = button(QWizard::FinishButton);
    finishButton->setEnabled(true);
}
