
#include "photos_view_widget.hpp"

#include <assert.h>

#include <memory>

#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QPixmap>
#include <QPainter>
#include <QScrollBar>
#include <QPushButton>
#include <QDebug>

#include "core/types.hpp"
#include "data/photo_info.hpp"

//useful links:
//http://www.informit.com/articles/article.aspx?p=1613548
//http://qt-project.org/doc/qt-5.1/qtcore/qabstractitemmodel.html
//http://qt-project.org/doc/qt-5.1/qtwidgets/qabstractitemview.html


namespace
{

    //TODO: remove, use config
    const int leftMargin = 20;
    const int rightMargin = 20;
    const int topMargin  = 20;
    const int imageMargin = 10;
    //
 

    struct ImagesModel: public QAbstractListModel
    {

        ImagesModel(): QAbstractListModel(), m_photos() {}

        ~ImagesModel()
        {
        }

        void add(const PhotoInfo &photoInfo)
        {
            QModelIndex parentIndex;
            const int items = m_photos.size();

            beginInsertRows(parentIndex, items, items);

            PhotoInfo::Ptr photo = std::make_shared<PhotoInfo>(photoInfo);
            m_photos.push_back(photo);

            endInsertRows();
        }

        PhotoInfo::Ptr get(const QModelIndex &idx)
        {
            const int row = idx.row();
            PhotoInfo::Ptr result = m_photos[row];

            return result;
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
            const PhotoInfo::Ptr info = m_photos[row];

            QVariant result;

            switch(role)
            {
                case Qt::DisplayRole:
                    result = info->getPath();
                    break;

                case Qt::DecorationRole:
                    result = info->getPixmap();
                    break;

                default:
                    break;
            }

            return result;
        }

        private:
            std::vector<PhotoInfo::Ptr> m_photos;
    };
    
    
    struct ImageManager
    {
        ImageManager(QAbstractItemModel *model): m_model(model) {}
        ImageManager(const ImageManager &) = delete;
        ~ImageManager() {}

        void operator=(const ImageManager &) = delete;
        
        QSize size(int i) const           //size of 'index' item in model
        {
            QPixmap image(getPixmap(i));

            //image size
            QSize imageSize = image.size();
            
            //add margins
            imageSize.rwidth() += imageMargin * 2;
            imageSize.rheight() += imageMargin * 2;
            
            return imageSize;
        }
        
        void draw(int i, QPainter *painter, const QRect &rect) const
        {
            QPixmap image(getPixmap(i));
            
            //image size
            QSize imageSize = image.size();
    
            QPoint center = rect.center();
            
            QRect target(center.x() - imageSize.width() / 2,
                         center.y() - imageSize.height() / 2,
                         imageSize.width(),
                         imageSize.height());
            
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

    
    struct PositionsCache
    {
            PositionsCache(QAbstractItemView *view): m_data(new MutableData(view)) {}
            ~PositionsCache() {}

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

            QRect pos(int i) const
            {
                validateCache();
                const int verticalOffset = m_data->m_view->verticalScrollBar()->value();

                QRect rect = m_data->m_pos[i];
                rect.moveTo(rect.x(), rect.y() - verticalOffset );

                return rect;
            }

        private:
            struct MutableData
            {
                MutableData(QAbstractItemView *view): m_valid(false), m_pos(), m_rows(), m_view(view), m_totalHeight(0) {}
                MutableData(const MutableData &) = delete;
                void operator=(const MutableData &) = delete;

                bool m_valid;
                std::vector<QRect> m_pos;         //position of items on grid
                std::vector<int>   m_rows;        //each row's height
                QAbstractItemView *m_view;
                int                m_totalHeight;
            };

            std::unique_ptr<MutableData> m_data;

            void flushData() const
            {
                m_data->m_pos.clear();
                m_data->m_rows.clear();
                m_data->m_totalHeight = 0;
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
                    const int baseX = 0;
                    const int width = m_data->m_view->viewport()->width();
                    int x = baseX;
                    int y = 0;
                    int rowHeight = 0;

                    flushData();
                    const int count = dataModel->rowCount(QModelIndex());
                    
                    ImageManager imageManager(dataModel);

                    for(int i = 0; i < count; i++)
                    {
                        //image size
                        QSize size = imageManager.size(i);
                        
                        //check if position is correct
                        if (x + size.width() >= width)      //no place? go to next row
                        {
                            assert(rowHeight > 0);
                            x = baseX;
                            y += rowHeight;

                            m_data->m_rows.push_back(rowHeight);
                            m_data->m_totalHeight += rowHeight;
                            rowHeight = 0;
                        }

                        //save position
                        QRect position(x, y, size.width(), size.height());
                        m_data->m_pos.push_back(position);
                        
                        x += size.width();
                        
                        rowHeight = std::max(rowHeight, size.height());
                    }

                    //save last row
                    m_data->m_rows.push_back(rowHeight);
                    m_data->m_totalHeight += rowHeight;

                    //update scroll bars
                    updateScrollBars();

                }
            }

            void updateScrollBars() const
            {
                QSize areaSize = m_data->m_view->viewport()->size();

                const int avail_height = areaSize.height();
                const int range_top = m_data->m_totalHeight - avail_height;

                m_data->m_view->verticalScrollBar()->setPageStep(avail_height);
                m_data->m_view->verticalScrollBar()->setRange(0, range_top);
            }
    };


    struct ImagesView: public QAbstractItemView
    {
        PositionsCache m_cache;

        explicit ImagesView(QWidget* p): QAbstractItemView(p), m_cache(this) {}
        
        QModelIndexList getSelection() const
        {
            return selectedIndexes();
        }

        //QWidget's virtuals:
        virtual void paintEvent(QPaintEvent* )
        {
            //TODO: use itemDelegate() for painting
            QPainter painter(viewport());

            const int items = m_cache.items();
            QAbstractItemModel *dataModel = model();
            ImageManager imageManager(dataModel);
            
            for (int i = 0; i < items; i++)
            {
                QModelIndex index = model()->index(i, 0);
                const QRect position = m_cache.pos(i);

                //paint selection
                const bool selected = selectionModel()->isSelected(index);

                if (selected)
                {
                    painter.setPen(QColor(0, 0, 0, 0));
                    painter.setBrush(QBrush(QColor(0, 0, 255)));
                    painter.drawRect(position);
                }

                //paint image
                imageManager.draw(i, &painter, position);
            }
        }
        
        virtual void resizeEvent(QResizeEvent *e)
        {
            m_cache.invalidate();
            QAbstractItemView::resizeEvent(e);
        }

        //QAbstractItemView's pure virtuals:
        virtual QRect visualRect(const QModelIndex& index) const
        {
            QAbstractItemModel *dataModel = model();
            QRect result;

            if (dataModel != nullptr && index.isValid())
            {
                const int row = index.row();
                result = m_cache.pos(row);
            }

            return result;
        }

        virtual void scrollTo(const QModelIndex& idx, ScrollHint hint = EnsureVisible)
        {
            (void) idx;
            (void) hint;
        }

        virtual QModelIndex indexAt(const QPoint& point) const
        {
            QModelIndex result;
            for(size_t i = 0; i < m_cache.items(); i++)
            {
                const QRect &position = m_cache.pos(i);

                if (position.contains(point))
                {
                    result = model()->index(i, 0);
                    break;
                }
            }

            qDebug() << result;

            return result;
        }

        virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
        {
            (void) cursorAction;
            (void) modifiers;
            QModelIndex result;

            return result;
        }

        virtual int horizontalOffset() const
        {
            return 0;
        }

        virtual int verticalOffset() const
        {
            return verticalScrollBar()->value();
        }

        virtual bool isIndexHidden(const QModelIndex& index) const
        {
            (void) index;
            return false;
        }

        virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
        {            
            QItemSelection selection;

            //find all items in rect
            for(size_t i = 0; i < m_cache.items(); i++)
            {
                QRect item = m_cache.pos(i);

                if ( (rect & item).isEmpty() == false )
                {
                    QModelIndex index = model()->index(i, 0);

                    selection.select(index, index);
                }
            }

            selectionModel()->select(selection, command);
        }

        virtual QRegion visualRegionForSelection(const QItemSelection& selection) const
        {
            (void) selection;
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


GuiDataSlots::GuiDataSlots(QObject *p): QObject(p) {}
GuiDataSlots::~GuiDataSlots() {}


struct PhotosViewWidget::GuiData: private GuiDataSlots
{
        GuiData(PhotosViewWidget *editor): GuiDataSlots(editor), m_editor(editor), m_photosModel(), m_photosView(nullptr)
        {
            m_photosView = new ImagesView(m_editor);
            m_photosView->setModel(&m_photosModel);

            QVBoxLayout *layout = new QVBoxLayout(m_editor);
            layout->addWidget(m_photosView);
            
            QHBoxLayout *savePhotosLayout = new QHBoxLayout;
            layout->addLayout(savePhotosLayout);
            
            QPushButton* saveButton = new QPushButton(tr("save photos"));
            savePhotosLayout->addStretch(1);
            savePhotosLayout->addWidget(saveButton);

            connect(m_photosView->selectionModel(),
                    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                    this,
                    SLOT(selectionChanged())
                    );
            
            connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(savePhotos()));
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
        PhotosViewWidget *m_editor;

        ImagesModel m_photosModel;
        ImagesView *m_photosView;

        void selectionChanged() override
        {
            std::vector<PhotoInfo::Ptr> images;
            
            //collect list of tags
            for (const QModelIndex& index: m_photosView->getSelection())
            {
                PhotoInfo::Ptr photoInfo = m_photosModel.get(index);
                images.push_back(photoInfo);
            }
            
            emit m_editor->selectionChanged(images);
        }
        
        void savePhotos() override
        {
            
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
