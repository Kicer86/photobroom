
#ifndef PREPARING_PHOTOS_WIDGET_H
#define PREPARING_PHOTOS_WIDGET_H

#include <QWidget>

#include <database/iphoto_info.hpp>

namespace Database
{
    struct IDatabase;
}

class QItemSelection;

class TagEditorWidget;
class PhotosViewWidget;
struct PhotosReceiver;
struct ITaskExecutor;
struct IConfiguration;

class PhotosStagingArea: public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosStagingArea(QWidget * = 0);
        virtual ~PhotosStagingArea();

        PhotosStagingArea(const PhotosStagingArea &) = delete;
        void operator=(const PhotosStagingArea &) = delete;
        void setDatabase(Database::IDatabase *);
        void set(ITaskExecutor *);
        void set(IConfiguration *);

    private:
        PhotosViewWidget *m_editor;
        TagEditorWidget  *m_tagEditor;
        std::unique_ptr<PhotosReceiver> m_photosReceiver;

    private slots:
        void pathToAnalyze(const QString &);
        void viewSelectionChanged(const std::vector<IPhotoInfo::Ptr >&);
        void savePhotos();

};

#endif // PREPARING_PHOTOS_WIDGET_H
