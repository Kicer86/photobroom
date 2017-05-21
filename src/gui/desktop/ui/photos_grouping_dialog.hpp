
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include <database/iphoto_info.hpp>


struct IPhotosManager;
struct IExifReader;
struct ITaskExecutor;

namespace Ui
{
    class PhotosGroupingDialog;
}

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
        explicit PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr> &, IExifReader *, ITaskExecutor *, QWidget *parent = 0);
        PhotosGroupingDialog(const PhotosGroupingDialog &) = delete;
        ~PhotosGroupingDialog();

        PhotosGroupingDialog& operator=(const PhotosGroupingDialog &) = delete;

        QString getRepresentative() const;

        void reject() override;

    private:
        QStandardItemModel m_model;
        std::unique_ptr<QMovie> m_movie;
        SortingProxy m_sortProxy;
        QString m_representativeFile;
        Ui::PhotosGroupingDialog *ui;
        IExifReader* m_exifReader;
        ITaskExecutor* m_executor;
        bool m_workInProgress;

        void generationTitle(const QString &);
        void generationProgress(int);
        void generationDone(const QString &);
        void refreshDialogButtons();
        void typeChanged();
        void applyPressed();
        void makeAnimation();
        void fillModel(const std::vector<IPhotoInfo::Ptr> &);

        QStringList getPhotos() const;

    // internal signals:
    signals:
        void cancel();
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
