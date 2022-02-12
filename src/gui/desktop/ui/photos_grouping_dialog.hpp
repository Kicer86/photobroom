
#ifndef PHOTOS_GROUPING_DIALOG_HPP
#define PHOTOS_GROUPING_DIALOG_HPP

#include <QDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include <database/photo_data.hpp>
#include "utils/grouppers/generator_utils.hpp"


class MediaPreview;
class Project;
struct IConfiguration;
struct IExifReaderFactory;
struct ILogger;
struct ITaskExecutor;
struct ITmpDir;

namespace Ui
{
    class PhotosGroupingDialog;
}

namespace Database
{
    struct IDatabase;
}

class SortingProxy: public QSortFilterProxyModel
{
    public:
        explicit SortingProxy(QObject* p = nullptr): QSortFilterProxyModel(p) {}

    protected:
        bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
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
        explicit PhotosGroupingDialog(const std::vector<Photo::Data> &,
                                      IExifReaderFactory &,
                                      ITaskExecutor &,
                                      IConfiguration &,
                                      ILogger *,
                                      Group::Type = Group::Type::Invalid,
                                      QWidget *parent = 0);

        PhotosGroupingDialog(const PhotosGroupingDialog &) = delete;
        ~PhotosGroupingDialog();

        PhotosGroupingDialog& operator=(const PhotosGroupingDialog &) = delete;

        QString getRepresentative() const;
        Group::Type groupType() const;

        const std::vector<Photo::Data>& photos() const;

        void reject() override;

    private:
        QStandardItemModel m_model;
        std::shared_ptr<ITmpDir> m_tmpDir;
        SortingProxy m_sortProxy;
        QString m_representativeFile;
        std::vector<Photo::Data> m_photos;
        Group::Type m_representativeType;
        Ui::PhotosGroupingDialog *ui;
        MediaPreview* m_preview;
        IExifReaderFactory& m_exifReaderFactory;
        IConfiguration& m_config;
        ILogger* m_logger;
        ITaskExecutor& m_executor;
        bool m_workInProgress;

        void generationTitle(const QString &);
        void generationProgress(int);
        void generationDone(const QString &);
        void generationCanceled();
        void generationError(const QString &, const QStringList &);
        void refreshDialogButtons();
        void previewPressed();
        void previewCancelPressed();
        void makeAnimation();
        void makeHDR();
        void makeCollage();
        void fillModel(const std::vector<Photo::Data> &);
        double calculateFPS() const;
        void startTask(std::unique_ptr<GeneratorUtils::BreakableTask>);
        void switchUiToGeneration();
        void switchUiToGenerationFinished();

        QStringList getPhotos() const;
        void scalePreview();

    // internal signals:
    signals:
        void cancel();
};

#endif // PHOTOS_GROUPING_DIALOG_HPP
