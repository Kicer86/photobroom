
#include "photos_editor_widget.hpp"

#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QPixmap>

namespace
{
    struct PhotoInfo
    {
        PhotoInfo(const QString &p): pixmap(p), path(p)
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

        PhotosModel(): QAbstractListModel(), m_photos() {}

        std::vector<PhotoInfo> m_photos;

        void add(const PhotoInfo &photoInfo)
        {
            QModelIndex parentIndex;
            const int items = m_photos.size();
            
            beginInsertRows(parentIndex, items, items);
            
            m_photos.push_back(photoInfo);
            
            endInsertRows();
        }
        
        //QAbstractItemModel:
        

        //QAbstractListModel:
        int rowCount(const QModelIndex &/*parent*/) const
        {
            return m_photos.size();
        }

        QVariant data(const QModelIndex &_index, int role) const
        {
            const int row = _index.row();
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

            return result;
        }
    };
    
    
    struct PhotosView: public QAbstractItemView
    {
        explicit PhotosView(QWidget* p): QAbstractItemView(p) {}
        
        virtual QRect visualRect(const QModelIndex& index) const
        {
        }
        
        virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible)
        {
        }
        
        virtual QModelIndex indexAt(const QPoint& point) const
        {
        }
        
        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
        {
        }
        
        virtual int horizontalOffset() const
        {
        }
        
        virtual int verticalOffset() const
        {
        }
        
        virtual bool isIndexHidden(const QModelIndex& index) const
        {
        }
        
        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
        {
        }
        
        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const
        {
        }
    };

}


struct PhotosEditorWidget::GuiData
{
    GuiData(QWidget *editor): m_editor(editor), m_photosModel(), m_photosView(nullptr)
    {
        m_photosView = new PhotosView(m_editor);
        m_photosView->setModel(&m_photosModel);

        QVBoxLayout *layout = new QVBoxLayout(m_editor);
        layout->addWidget(m_photosView);
    }

    GuiData(const GuiData &) = delete;
    ~GuiData() {}    
    void operator=(const GuiData &) = delete;

    void addPhoto(const std::string &path)
    {
        PhotoInfo info(path.c_str());

        m_photosModel.add(info);
    }

    private:
        QWidget *m_editor;

        PhotosModel m_photosModel;
        PhotosView *m_photosView;
};


PhotosEditorWidget::PhotosEditorWidget(QWidget *p): QWidget(p), m_gui(new GuiData(this))
{
}


PhotosEditorWidget::~PhotosEditorWidget()
{

}


void PhotosEditorWidget::addPhoto(const std::string &photo)
{
    m_gui->addPhoto(photo);
}
