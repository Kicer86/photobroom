
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTemporaryDir>

#include <database/iphoto_info.hpp>


struct IPhotosManager;
struct IExifReader;

namespace Ui
{
    class PhotosGroupingDialog;
}

struct AnimationGenerator;

class SortingProxy: public QSortFilterProxyModel
{
    public:
        SortingProxy(QObject* p = nullptr): QSortFilterProxyModel(p) {}

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const
        {
            if (left.column() == 1)
            {
                const QVariant lhs = left.data(Qt::DisplayRole);
                const QVariant rhs = right.data(Qt::DisplayRole);

                const int left_value = lhs.toInt();
                const int right_value = rhs.toInt();

                return left_value < right_value;
            }
            else
                return QSortFilterProxyModel::lessThan(left, right);
        }
};


class PhotosGroupingDialog: public QDialog
{
        Q_OBJECT

    public:
        explicit PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr> &, IExifReader *, QWidget *parent = 0);
        PhotosGroupingDialog(const PhotosGroupingDialog &) = delete;
        ~PhotosGroupingDialog();

        PhotosGroupingDialog& operator=(const PhotosGroupingDialog &) = delete;

    private:
        QStandardItemModel m_model;
        QTemporaryDir m_tmpLocation;
        std::unique_ptr<AnimationGenerator> m_animationGenerator;
        SortingProxy m_sortProxy;
        Ui::PhotosGroupingDialog *ui;
        IExifReader* m_exifReader;

        void updatePreview(QWidget *);
        void typeChanged();
        void makeAnimation();
        void fillModel(const std::vector<IPhotoInfo::Ptr> &);

        QStringList getPhotos() const;
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
