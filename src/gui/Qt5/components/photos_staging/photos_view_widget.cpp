
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

#include <OpenLibrary/QtExt/qtext_choosefile.hpp>

#include <core/tag.hpp>
#include <core/photo_info.hpp>
#include <database/databasebuilder.hpp>

#include "components/photos_staging/staging_data_model.hpp"
#include "model_view/images_tree_view.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


BrowseLine::BrowseLine(QWidget *p):
    QWidget(p),
    m_dialog(nullptr),
    m_button(nullptr),
    m_line(nullptr),
    m_addButton(nullptr),
    m_chooser(nullptr)
{
    m_button = new QPushButton(tr("Browse"), this);
    m_line = new QLineEdit(this);
    m_addButton = new QPushButton(tr("Add"));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    mainLayout->addWidget(new QLabel(tr("Path to photos:"), this));
    mainLayout->addWidget(m_line);
    mainLayout->addWidget(m_button);
    mainLayout->addWidget(m_addButton);

    connect(m_button, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));

    //prepare dialog for choosing files
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    m_chooser = new QtExtChooseFile(m_button, m_line, fileDialog);
}


BrowseLine::~BrowseLine()
{

}


void BrowseLine::browseButtonClicked()
{

}


void BrowseLine::addButtonClicked()
{
    const QString path = m_line->text();

    emit addPath(path);
}


BrowseList::BrowseList(QWidget *p): QWidget(p)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    BrowseLine *line = new BrowseLine(this);
    mainLayout->addWidget(line);
    connect(line, SIGNAL(addPath(QString)), this, SIGNAL(addPath(QString)));
}


BrowseList::~BrowseList()
{
}


PhotosViewWidget::PhotosViewWidget(QWidget *p): QWidget(p), m_photosModel(nullptr), m_photosView(nullptr)
{
    Database::IDatabase* db = Database::Builder::instance()->get();
    m_photosModel = new StagingDataModel(this);
    m_photosModel->setDatabase(db);

    m_photosView = new ImagesTreeView(this);
    m_photosView->setModel(m_photosModel);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(m_photosView);

    connect(m_photosView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionChanged())
           );
}


PhotosViewWidget::~PhotosViewWidget()
{

}


void PhotosViewWidget::addPhoto(const QString &path)
{
    PhotoInfo::Ptr info = std::make_shared<PhotoInfo>(path);
    info->markStagingArea();

    m_photosModel->addPhoto(info);
}


std::vector<PhotoInfo::Ptr> PhotosViewWidget::getPhotos() const
{
    return m_photosModel->getPhotos();
}


void PhotosViewWidget::storePhotos()
{
    m_photosModel->storePhotos();
}


void PhotosViewWidget::selectionChanged()
{
    std::vector<PhotoInfo::Ptr> images;

    //collect list of tags
    for (const QModelIndex& index: m_photosView->selectionModel()->selectedIndexes())
    {
        PhotoInfo::Ptr photoInfo = m_photosModel->getPhoto(index);
        images.push_back(photoInfo);
    }

    emit selectionChanged(images);
}
