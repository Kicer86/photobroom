
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>
#include <QTemporaryDir>

#include <database/iphoto_info.hpp>

#include "models/photo_info_list_model.hpp"


struct IThumbnailAcquisitor;

namespace Ui
{
    class PhotosGroupingDialog;
}

struct IGroupingGenerator
{
    virtual ~IGroupingGenerator() = default;
};


class PhotosGroupingDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr> &, IThumbnailAcquisitor *, QWidget *parent = 0);
        PhotosGroupingDialog(const PhotosGroupingDialog &) = delete;
        ~PhotosGroupingDialog();

        PhotosGroupingDialog& operator=(const PhotosGroupingDialog &) = delete;

        void set(const std::vector<IPhotoInfo::Ptr> &);

    private:
        PhotoInfoListModel m_model;
        std::unique_ptr<IGroupingGenerator> m_generator;
        QTemporaryDir m_tmpLocation;
        Ui::PhotosGroupingDialog *ui;

        void updatePreview(QWidget *);
        void typeChanged();
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
