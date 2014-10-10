
#include "photos_adding_wizard.hpp"


#include <QFileDialog>

#include <OpenLibrary/QtExt/qtext_choosefile.hpp>

#include <analyzer/photo_crawler_builder.hpp>
#include <analyzer/iphoto_crawler.hpp>

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


    struct PhotosReceiver: IMediaNotification
    {
        PhotosReceiver(): m_model(nullptr) {}
        PhotosReceiver(const PhotosReceiver &) = delete;

        PhotosReceiver& operator=(const PhotosReceiver &) = delete;

        void setModel(StagedPhotosDataModel* model)
        {
            m_model = model;
        }

        virtual void found(const QString& path) override
        {
            m_model->addPhoto(path);
        }

        StagedPhotosDataModel* m_model;
    };
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
        on_photosLocation_textChanged("");
    else if (id == 1)
    {
        //run crawler
        const QString path = ui->photosLocation->text();

        IPhotoCrawler* crawler = PhotoCrawlerBuilder().build();
        crawler->crawl(path, m_mediaNotification.get());
    }
}
