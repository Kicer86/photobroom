
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QProcess>
#include <QMovie>
#include <QTemporaryFile>

#include "ui_photos_grouping_dialog.h"
#include "ui_utils/lazy_tree_item_delegate.hpp"


namespace
{

    struct AnimationGenerator: QObject, IGroupingGenerator
    {
        AnimationGenerator(const std::function<void(QWidget *)>& callback, const QString& location):
            m_callback(callback),
            m_photos(),
            m_location(location),
            m_movie(),
            m_fps(10),
            m_scale(1),
            m_enabled(false)
        {
        }

        void setPhotos(const std::vector<IPhotoInfo::Ptr>& photos)
        {
            m_photos = photos;

            generatePreviewWidget();
        }

        void setFps(double fps)
        {
            m_fps = fps;

            generatePreviewWidget();
        }

        void setScale(double scale)
        {
            m_scale = scale;

            generatePreviewWidget();
        }


        void enable()
        {
            m_enabled = true;

            generatePreviewWidget();
        }


        void generatePreviewWidget()
        {
            if (m_enabled)
            {
                assert(m_photos.empty() == false);
                assert(m_location.isEmpty() == false);

                const QString location = QString("%1/animation.gif").arg(m_location);

                QStringList images;

                for(const IPhotoInfo::Ptr& photo: m_photos)
                {
                    const QString path = photo->getPath();
                    const QFileInfo fileInfo(path);
                    const QString absoluteFilePath = fileInfo.absoluteFilePath();

                    images.append(absoluteFilePath);
                }

                QStringList args;
                args << "-delay" << QString::number(1/m_fps * 100);   // convert fps to 1/100th of a second
                args << images;
                args << "-loop" << "0";
                args << "-resize" << QString::number(100/m_scale) + "%";
                args << location;

                QProcess convert;
                convert.start("convert", args);
                convert.waitForFinished(-1);

                m_movie = std::make_unique<QMovie>(location);
                QLabel* label = new QLabel;

                label->setMovie(m_movie.get());
                m_movie->start();

                m_callback(label);
            }
        }

        std::function<void(QWidget *)> m_callback;
        std::vector<IPhotoInfo::Ptr> m_photos;
        const QString m_location;
        std::unique_ptr<QMovie> m_movie;
        double m_fps;
        double m_scale;
        bool m_enabled;
    };

}



PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr>& photos, IThumbnailAcquisitor* th_acq, QWidget *parent):
    QDialog(parent),
    m_model(),
    m_generator(),
    m_tmpLocation(),
    ui(new Ui::PhotosGroupingDialog)
{
    m_model.set(photos);

    ui->setupUi(this);
    ui->photosView->setModel(&m_model);

    LazyTreeItemDelegate* delegate = new LazyTreeItemDelegate(ui->photosView);
    delegate->set(th_acq);

    ui->photosView->setItemDelegate(delegate);

    typeChanged();
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


void PhotosGroupingDialog::updatePreview(QWidget* preview)
{
    if (preview != nullptr)
        ui->resultPreview->setWidget(preview);
}


void PhotosGroupingDialog::typeChanged()
{
    const int current = ui->groupingType->currentIndex();

    if (current == 0)
    {
        using namespace std::placeholders;
        auto callback = std::bind(&PhotosGroupingDialog::updatePreview, this, _1);

        auto generator = std::make_unique<AnimationGenerator>(callback, m_tmpLocation.path());
        generator->setPhotos(m_model.getPhotos());
        generator->setFps(ui->speedSpinBox->value());
        generator->setScale(ui->scaleSpinBox->value());

        auto speedSignal = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
        QObject::connect(ui->speedSpinBox, speedSignal, generator.get(), &AnimationGenerator::setFps);

        auto scaleSignal = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
        QObject::connect(ui->scaleSpinBox, scaleSignal, generator.get(), &AnimationGenerator::setScale);

        generator->enable();

        m_generator = std::move(generator);
    }
}
