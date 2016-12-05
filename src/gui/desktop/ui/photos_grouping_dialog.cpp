
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QProcess>
#include <QMovie>
#include <QTemporaryFile>

#include "ui_photos_grouping_dialog.h"
#include "ui_utils/lazy_tree_item_delegate.hpp"


struct AnimationGenerator: QObject
{
    struct Data
    {
        double fps;
        double delay;
        double scale;
        std::vector<IPhotoInfo::Ptr> photos;
    };

    AnimationGenerator(const std::function<void(QWidget *)>& callback, const QString& location):
        m_callback(callback),
        m_location(location),
        m_movie()
    {
    }


    void generatePreviewWidget(const Data& data)
    {
        assert(m_location.isEmpty() == false);

        const QString location = QString("%1/animation.gif").arg(m_location);

        QStringList images;

        for(const IPhotoInfo::Ptr& photo: data.photos)
        {
            const QString path = photo->getPath();
            const QFileInfo fileInfo(path);
            const QString absoluteFilePath = fileInfo.absoluteFilePath();

            images.append(absoluteFilePath);
        }

        QStringList args;
        args << "-delay" << QString::number(1/data.fps * 100);   // convert fps to 1/100th of a second
        args << images;
        args << "-loop" << "0";
        args << "-resize" << QString::number(100/data.scale) + "%";
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

    std::function<void(QWidget *)> m_callback;
    const QString m_location;
    std::unique_ptr<QMovie> m_movie;
};


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr>& photos, IThumbnailAcquisitor* th_acq, QWidget *parent):
    QDialog(parent),
    m_model(),
    m_tmpLocation(),
    m_animationGenerator(),
    ui(new Ui::PhotosGroupingDialog)
{
    m_model.set(photos);

    ui->setupUi(this);
    ui->photosView->setModel(&m_model);

    LazyTreeItemDelegate* delegate = new LazyTreeItemDelegate(ui->photosView);
    delegate->set(th_acq);

    ui->photosView->setItemDelegate(delegate);

    using namespace std::placeholders;
    auto callback = std::bind(&PhotosGroupingDialog::updatePreview, this, _1);

    m_animationGenerator = std::make_unique<AnimationGenerator>(callback, m_tmpLocation.path());
    connect(ui->applyButton, &QPushButton::pressed, this, &PhotosGroupingDialog::makeAnimation);

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
        makeAnimation();
}


void PhotosGroupingDialog::makeAnimation()
{
    AnimationGenerator::Data generator_data;

    generator_data.photos = m_model.getPhotos();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();

    m_animationGenerator->generatePreviewWidget(generator_data);
}
