
#include "photos_editor_widget.hpp"

#include <QVBoxLayout>
#include <QListView>
#include <QAbstractListModel>
#include <QPixmap>

namespace
{
    struct PhotoInfo
    {
        PhotoInfo(const QString &path): pixmap(path), path(path)
        {
        }

        QPixmap pixmap;
        QString path;
    };

    //TODO: remove, use config
    const int photoWidth = 120;
    const int leftMargin = 20;
    const int rightMargin = 20;
    const int topMargin  = 20;
    //

    struct PhotosModel: public QAbstractListModel
    {
        std::vector<PhotoInfo> m_photos;

        void add(const PhotoInfo &photoInfo)
        {
            m_photos.push_back(photoInfo);
        }

        //QAbstractListModel:
        int rowCount(const QModelIndex &parent) const
        {
            return m_photos.size();
        }

        QVariant data(const QModelIndex &index, int role) const
        {
            const int row = index.row();
            const PhotoInfo &info = m_photos[row];

            QVariant result;

            switch(role)
            {
                case Qt::DisplayRole:
                    result = info.path;
                    break;

                case Qt::DecorationRole:
                    result = info.pixmap;
                    break;

                default:
                    break;
            }
        }
    };

}


struct PhotosEditorWidget::GuiData
{
    GuiData(QWidget *editor): m_editor(editor), m_photosModel(), m_photosView(nullptr)
    {
        m_photosView = new QListView(m_editor);
        m_photosView->setModel(&m_photosModel);

        QVBoxLayout *layout = new QVBoxLayout(m_editor);
        layout->addWidget(m_photosView);
    }

    ~GuiData() {}

    void addPhoto(const std::string &path)
    {
        PhotoInfo info(path.c_str());

        m_photosModel.add(info);
    }

    private:
        QWidget *m_editor;

        PhotosModel m_photosModel;
        QListView *m_photosView;
};


PhotosEditorWidget::PhotosEditorWidget(QWidget *parent): QWidget(parent), m_gui(new GuiData(this))
{
}


PhotosEditorWidget::~PhotosEditorWidget()
{

}


void PhotosEditorWidget::addPhoto(const std::string &photo)
{
    m_gui->addPhoto(photo);
}
