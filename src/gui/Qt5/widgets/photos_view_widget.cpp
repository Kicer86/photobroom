
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QVBoxLayout>
#include <QPixmap>
#include <QPushButton>

#include <core/tag.hpp>
#include <core/photo_info.hpp>
#include <database/databasebuilder.hpp>

#include "model_view/db_data_model.hpp"
#include "model_view/images_tree_view.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


PhotosViewWidget::PhotosViewWidget(QWidget *p): QWidget(p), m_photosModel(nullptr), m_photosView(nullptr)
{
    Database::IBackend* backend = Database::Builder::instance()->getBackend();
    m_photosModel = new DBDataModel(this);
    m_photosModel->setBackend(backend);

    m_photosView = new ImagesTreeView(this);
    m_photosView->setModel(m_photosModel);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(m_photosView);

    connect(m_photosView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChanged())
            );
}


PhotosViewWidget::~PhotosViewWidget()
{

}


void PhotosViewWidget::addPhoto(const std::string &path)
{
    PhotoInfo::Ptr info = std::make_shared<PhotoInfo>(path);

    QPixmap tmpThumbnail;
    tmpThumbnail.load(":/gui/images/clock.svg");             //use temporary thumbnail until final one is ready
    info->setTemporaryThumbnail(tmpThumbnail);

    m_photosModel->addPhoto(info);
}


std::vector<PhotoInfo::Ptr> PhotosViewWidget::getPhotos() const
{
    return m_photosModel->getPhotos();
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
