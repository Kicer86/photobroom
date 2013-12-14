
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QVBoxLayout>
#include <QPixmap>
#include <QPushButton>

#include "core/tag.hpp"
#include "core/photo_info.hpp"
#include "model_view/images_model.hpp"
#include "model_view/images_view.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}


GuiDataSlots::GuiDataSlots(QObject *p): QObject(p) {}
GuiDataSlots::~GuiDataSlots() {}


namespace
{
    struct PhotoManipulator: public PhotoInfo<QPixmap>::IManipulator
    {
        PhotoManipulator(): m_photo(nullptr), m_thumbnail(nullptr), m_photoRaw(), m_thumbnailRaw() {}

        PhotoManipulator(const PhotoManipulator &) = delete;
        PhotoManipulator& operator=(const PhotoManipulator &) = delete;

        virtual void set(QPixmap* photoPixmap, QPixmap* thumbnailPixmap) override
        {
            m_photo = photoPixmap;
            m_thumbnail = thumbnailPixmap;
        }

        virtual void load(const std::string& path) override
        {
            bool status = true;

            status = m_photo->load(path.c_str());
            m_photoRaw = m_photo->toImage();
            assert(status);

            //to do: thread
            //*m_thumbnail = m_photo->scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            status = m_thumbnail->load(":/gui/images/clock64.png");
            m_thumbnailRaw = m_thumbnail->toImage();
            assert(status);
        }

        virtual RawPhotoData rawPhoto() override
        {
            RawPhotoData data;

            data.data = m_photoRaw.bits();
            data.size = m_photoRaw.byteCount();

            return data;
        }

        virtual RawPhotoData rawThumbnail() override
        {
            RawPhotoData data;

            data.data = m_thumbnailRaw.bits();
            data.size = m_thumbnailRaw.byteCount();

            return data;
        }

        QPixmap* m_photo;
        QPixmap* m_thumbnail;

        QImage m_photoRaw;
        QImage m_thumbnailRaw;
    };
}


struct PhotosViewWidget::GuiData: private GuiDataSlots
{
        GuiData(PhotosViewWidget *editor): GuiDataSlots(editor), m_editor(editor), m_photosModel(), m_photosView(nullptr)
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
            APhotoInfo::Ptr info =
                std::make_shared<PhotoInfo<QPixmap>>(path.c_str(), new PhotoManipulator);

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

