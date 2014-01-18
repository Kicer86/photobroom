
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QVBoxLayout>
#include <QPixmap>
#include <QPushButton>

#include "core/tag.hpp"
#include "core/aphoto_info.hpp"
#include "model_view/images_model.hpp"
#include "model_view/images_view.hpp"
#include "photo_info.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


GuiDataSlots::GuiDataSlots(QObject *p): QObject(p) {}
GuiDataSlots::~GuiDataSlots() {}


struct PhotosViewWidget::GuiData: private GuiDataSlots
{
        GuiData(PhotosViewWidget *editor):
            GuiDataSlots(editor),
            m_editor(editor),
            m_photosModel(),
            m_photosView(nullptr)
        {
            m_photosView = new ImagesView(m_editor);
            m_photosView->setModel(&m_photosModel);

            QVBoxLayout *layout = new QVBoxLayout(m_editor);
            layout->addWidget(m_photosView);

            connect(m_photosView->selectionModel(),
                    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                    this,
                    SLOT(selectionChanged())
                    );
        }

        GuiData(const GuiData &) = delete;
        ~GuiData() {}
        void operator=(const GuiData &) = delete;

        void addPhoto(const std::string &path)
        {
            APhotoInfo::Ptr info = std::make_shared<PhotoInfo>(path, &m_photosModel);

            m_photosModel.add(info);
        }

        const std::vector<APhotoInfo::Ptr>& getAllPhotos() const
        {
            return m_photosModel.getAll();
        }

    private:
        PhotosViewWidget *m_editor;

        ImagesModel m_photosModel;
        ImagesView *m_photosView;

        void selectionChanged() override
        {
            std::vector<APhotoInfo::Ptr> images;

            //collect list of tags
            for (const QModelIndex& index: m_photosView->getSelection())
            {
                APhotoInfo::Ptr photoInfo = m_photosModel.get(index);
                images.push_back(photoInfo);
            }

            emit m_editor->selectionChanged(images);
        }
};


PhotosViewWidget::PhotosViewWidget(QWidget *p): QWidget(p), m_gui(new GuiData(this))
{
}


PhotosViewWidget::~PhotosViewWidget()
{

}


void PhotosViewWidget::addPhoto(const std::string &photo)
{
    m_gui->addPhoto(photo);
}


const std::vector<APhotoInfo::Ptr>& PhotosViewWidget::getPhotos() const
{
    return m_gui->getAllPhotos();
}

