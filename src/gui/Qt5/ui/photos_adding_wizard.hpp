
#ifndef PHOTOS_ADDING_WIZARD_HPP
#define PHOTOS_ADDING_WIZARD_HPP

#include <memory>

#include <QWizard>

#include <analyzer/iphoto_crawler.hpp>

class QtExtChooseFile;

struct IMediaNotification;
struct IConfiguration;

class StagedPhotosDataModel;
namespace Ui
{
    class PhotosAddingWizard;
}

namespace Database
{
    struct IDatabase;
}


class PhotosReceiver: public QObject, public IMediaNotification
{
        Q_OBJECT

        StagedPhotosDataModel* m_model;

    public:
        PhotosReceiver();
        PhotosReceiver(const PhotosReceiver &) = delete;
        PhotosReceiver& operator=(const PhotosReceiver &) = delete;

        void setModel(StagedPhotosDataModel* model);
        virtual void found(const QString& path) override;

    signals:
        void finished() override;
};


class PhotosAddingWizard : public QWizard
{
    Q_OBJECT

    public:
        explicit PhotosAddingWizard(QWidget *parent = 0);
        PhotosAddingWizard(const PhotosAddingWizard &) = delete;
        ~PhotosAddingWizard();

        void set(Database::IDatabase *);
        void set(IConfiguration *);

        PhotosAddingWizard& operator=(const PhotosAddingWizard &) = delete;

    private slots:
        void on_photosLocation_textChanged(const QString &);
        void on_PhotosAddingWizard_currentIdChanged(int id);

    private:
        Ui::PhotosAddingWizard* ui;
        std::unique_ptr<QtExtChooseFile> m_chooseFile;
        std::unique_ptr<IMediaNotification> m_mediaNotification;
};

#endif // PHOTOS_ADDING_WIZARD_HPP
