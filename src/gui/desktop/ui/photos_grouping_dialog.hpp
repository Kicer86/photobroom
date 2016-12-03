
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>

#include <database/iphoto_info.hpp>

#include "models/photo_info_list_model.hpp"


namespace Ui {
class PhotosGroupingDialog;
}

class PhotosGroupingDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit PhotosGroupingDialog(QWidget *parent = 0);
        PhotosGroupingDialog(const PhotosGroupingDialog &) = delete;
        ~PhotosGroupingDialog();

        PhotosGroupingDialog& operator=(const PhotosGroupingDialog &) = delete;

        void set(const std::vector<IPhotoInfo::Ptr> &);

    private:
        PhotoInfoListModel m_model;
        Ui::PhotosGroupingDialog *ui;
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
