#ifndef STAGED_PHOTOS_VIEW_HPP
#define STAGED_PHOTOS_VIEW_HPP

#include <QWidget>

namespace Ui {
class StagedPhotosView;
}

class StagedPhotosView : public QWidget
{
    Q_OBJECT

public:
    explicit StagedPhotosView(QWidget *parent = 0);
    ~StagedPhotosView();

private:
    Ui::StagedPhotosView *ui;
};

#endif // STAGED_PHOTOS_VIEW_HPP
