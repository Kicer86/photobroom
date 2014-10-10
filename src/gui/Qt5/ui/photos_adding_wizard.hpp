
#ifndef PHOTOS_ADDING_WIZARD_HPP
#define PHOTOS_ADDING_WIZARD_HPP

#include <memory>

#include <QWizard>

class QtExtChooseFile;

struct IMediaNotification;
struct IConfiguration;

namespace Ui
{
    class PhotosAddingWizard;
}

namespace Database
{
    struct IDatabase;
}


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
