#ifndef PHOTOSVIEW_HPP
#define PHOTOSVIEW_HPP

#include <QWidget>

struct IConfiguration;
class DBDataModel;

namespace Ui
{
    class PhotosView;
}

class PhotosView : public QWidget
{
        Q_OBJECT

    public:
        explicit PhotosView(QWidget *parent = 0);
        PhotosView(const PhotosView &) = delete;
        ~PhotosView();

        PhotosView& operator=(const PhotosView &) = delete;

        void set(IConfiguration *);
        void setModel(DBDataModel *);

    private:
        Ui::PhotosView *ui;
        DBDataModel* m_imagesModel;

    private slots:
        void on_sortingCombo_currentIndexChanged(int index);
};

#endif // PHOTOSVIEW_HPP
