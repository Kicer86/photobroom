
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QVBoxLayout>
#include <QPixmap>
#include <QPushButton>

#include <core/tag.hpp>
#include <core/aphoto_info.hpp>
#include <database/databasebuilder.hpp>

#include "model_view/db_data_model.hpp"
#include "model_view/images_tree_view.hpp"
#include "data/photo_info_generator.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


PhotosViewWidget::PhotosViewWidget(QWidget *p): QWidget(p), m_photosModel(nullptr), m_photosView(nullptr)
{
    Database::IBackend* backend = Database::Builder::instance()->getBackend(Database::Builder::Temporary);
    m_photosModel = new DBDataModel(this);
    m_photosModel->setBackend(backend);

    m_photosView = new ImagesTreeView(this);
    m_photosView->setModel(m_photosModel);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_photosView);

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
    APhotoInfo::Ptr info = PhotoInfoGenerator().get(path);

    m_photosModel->addPhoto(info);
}


std::vector<APhotoInfo::Ptr> PhotosViewWidget::getPhotos() const
{
    return m_photosModel->getPhotos();
}


void PhotosViewWidget::selectionChanged()
{
    std::vector<APhotoInfo::Ptr> images;

    //collect list of tags
    for (const QModelIndex& index: m_photosView->selectionModel()->selectedIndexes())
    {
        APhotoInfo::Ptr photoInfo = m_photosModel->getPhoto(index);
        images.push_back(photoInfo);
    }

    emit selectionChanged(images);
}
