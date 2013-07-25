
#include "photos_editor_widget.hpp"

#include <assert.h>

#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QPixmap>
#include <QPainter>

//usefull links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html

namespace
{

    //TODO: remove, use config
    const int photoWidth = 120;
    const int leftMargin = 20;
    const int rightMargin = 20;
    const int topMargin  = 20;
    const int imageMargin = 10;
    //

    struct PhotoInfo
    {
        PhotoInfo(const QString &p): pixmap(), path(p)
        {
            QPixmap tmp(p);

            pixmap = tmp.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        QPixmap pixmap;
        QString path;
    };

    struct ImagesModel: public QAbstractListModel
    {

        ImagesModel(): QAbstractListModel(), m_photos() {}

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
    
    
    struct ImageManager
    {
        ImageManager(QAbstractItemModel *model): m_model(model) {}
        ~ImageManager() {}
        
        QSize size(int i) const           //size of 'index' item in model
        {
            QPixmap image(getPixmap(i));

            //image size
            QSize size = image.size();
            
            //add margins
            size.rwidth() += imageMargin * 2;
            size.rheight() += imageMargin * 2;
            
            return size;
        }
        
        void draw(int i, QPainter *painter, const QRect &rect) const
        {
            QPixmap image(getPixmap(i));
            
            //image size
            QSize size = image.size();
    
            QPoint center = rect.center();
            
            QRect target(center.x() - size.width() / 2,
                         center.y() - size.height() / 2,
                         size.width(),
                         size.height());
            
            painter->drawPixmap(target, image);
        }
        
        QPixmap getPixmap(int i) const
        {
            QModelIndex index = m_model->index(i, 0);
            QVariant variant = m_model->data(index, Qt::DecorationRole);

            QPixmap image = variant.value<QPixmap>();
            
            return image;
        }
        
        QAbstractItemModel *m_model;
    };

    
    struct Cache
    {
            Cache(QAbstractItemView *view): m_data(new MutableData(view)) {}
            ~Cache() {}

            void invalidate() const
            {
                m_data->m_valid = false;
                m_data->m_view->viewport()->update();
            }

            size_t items() const
            {
                validateCache();

                return m_data->m_pos.size();
            }

            const QRect& pos(int i) const
            {
                validateCache();
                return m_data->m_pos[i];
            }

        private:
            struct MutableData
            {
                MutableData(QAbstractItemView *view): m_valid(false), m_pos(), m_rows(), m_view(view) {}
                MutableData(const MutableData &) = delete;
                void operator=(const MutableData &) = delete;

                bool m_valid;
                std::vector<QRect> m_pos;         //position of items on grid
                std::vector<int>   m_rows;        //each row's height
                QAbstractItemView *m_view;
            };

            std::unique_ptr<MutableData> m_data;

            void flushData() const
            {
                m_data->m_pos.clear();
                m_data->m_rows.clear();
            }

            void validateCache() const
            {
                if (m_data->m_valid == false)
                {
                    reloadCache();
                    m_data->m_valid = true;
                }
            }

            void reloadCache() const
            {
                QAbstractItemModel *dataModel = m_data->m_view->model();

                if (dataModel != nullptr)
                {
                    const int baseX = m_data->m_view->viewport()->x();
                    const int width = m_data->m_view->viewport()->width();
                    int x = baseX;
                    int y = m_data->m_view->viewport()->y();
                    int rowHeight = 0;

                    flushData();
                    const int count = dataModel->rowCount(QModelIndex());
                    
                    ImageManager imageManager(dataModel);

                    for(int i = 0; i < count; i++)
                    {
                        //image size
                        QSize size = imageManager.size(i);

                        //save position
                        QRect position(x, y, size.width(), size.height());
                        m_data->m_pos.push_back(position);

                        //calculate nex position
                        if (x + size.width() >= width)
                        {
                            assert(rowHeight > 0);
                            x = baseX;
                            y += rowHeight;

                            m_data->m_rows.push_back(rowHeight);
                            rowHeight = 0;
                        }
                        else
                        {
                            x += size.width();
                            rowHeight = std::max(rowHeight, size.height());
                        }

                    }

                }
            }
    };


    struct ImagesView: public QAbstractItemView
    {
        Cache m_cache;

        explicit ImagesView(QWidget* p): QAbstractItemView(p), m_cache(this) {}

        //QWidget's virtuals:
        virtual void paintEvent(QPaintEvent* )
        {
            QPainter painter(viewport());

            const int items = m_cache.items();
            QAbstractItemModel *dataModel = model();
            ImageManager imageManager(dataModel);
            
            for (int i = 0; i < items; i++)
            {
                const QRect &position = m_cache.pos(i);
                imageManager.draw(i, &painter, position);
            }
        }
        
        virtual void resizeEvent(QResizeEvent* event)
        {
            m_cache.invalidate();
            QAbstractItemView::resizeEvent(event);
        }

        //QAbstractItemView's pure virtuals:
        virtual QRect visualRect(const QModelIndex& index) const
        {
            QAbstractItemModel *dataModel = model();
            QRect result;

            if (dataModel != nullptr)
            {
                const int row = index.row();
                result = m_cache.pos(row);
            }

            return result;
        }

        virtual void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible)
        {
        }

        virtual QModelIndex indexAt(const QPoint& point) const
        {
            QModelIndex result;

            return result;
        }

        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
        {
            QModelIndex result;

            return result;
        }

        virtual int horizontalOffset() const
        {
            return 0;
        }

        virtual int verticalOffset() const
        {
            return 0;
        }

        virtual bool isIndexHidden(const QModelIndex& index) const
        {
            return false;
        }

        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
        {
        }

        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const
        {
            QRegion result;

            return result;
        }

        //QAbstractItemView's virtuals:
        virtual void dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector< int >& roles = QVector<int>())
        {
            m_cache.invalidate();
            QAbstractItemView::dataChanged(topLeft, bottomRight, roles);
        }

        virtual void rowsInserted(const QModelIndex& parent, int start, int end)
        {
            m_cache.invalidate();
            QAbstractItemView::rowsInserted(parent, start, end);
        }

        virtual void rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
        {
            m_cache.invalidate();
            QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
        }
    };

}


struct PhotosEditorWidget::GuiData
{
        GuiData(QWidget *editor): m_editor(editor), m_photosModel(), m_photosView(nullptr)
        {
            m_photosView = new ImagesView(m_editor);
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

        ImagesModel m_photosModel;
        ImagesView *m_photosView;
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
