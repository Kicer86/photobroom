
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QProcess>
#include <QProgressBar>
#include <QMovie>

#include <core/iexif_reader.hpp>
#include <core/itask_executor.hpp>
#include <core/cross_thread_call.hpp>
#include <system/system.hpp>

#include "ui_photos_grouping_dialog.h"


struct AnimationGenerator: QObject
{
    struct Data
    {
        double fps;
        double delay;
        double scale;
        QStringList photos;

        Data(): fps(0.0), delay(0.0), scale(0.0), photos() {}
    };

    struct GifGenerator: ITaskExecutor::ITask
    {
        GifGenerator(const AnimationGenerator::Data& data, const QString& location, const std::function<void(const QString &)>& doneCallback):
            m_data(data),
            m_location(location),
            m_doneCallback(doneCallback)
        {
        }

        std::string name() const override
        {
            return "GifGenerator";
        }

        void perform() override
        {
            QStringList args;
            args << "-delay" << QString::number(1/m_data.fps * 100);   // convert fps to 1/100th of a second
            args << m_data.photos;
            args << "-loop" << "0";
            args << "-resize" << QString::number(100/m_data.scale) + "%";
            args << m_location;

            QProcess convert;
            convert.start("convert", args);
            convert.waitForFinished(-1);

            m_doneCallback(m_location);
        }

        AnimationGenerator::Data m_data;
        QString m_location;
        std::function<void(const QString &)> m_doneCallback;
    };

    AnimationGenerator(ITaskExecutor* executor, const std::function<void(QWidget *, const QString &)>& callback):
        m_callback(callback),
        m_movie(),
        m_baseSize(),
        m_executor(executor)
    {
    }

    AnimationGenerator(const AnimationGenerator &) = delete;
    AnimationGenerator& operator=(const AnimationGenerator &) = delete;


    void generatePreviewWidget(const Data& data)
    {
        if (m_movie.get() != nullptr)
            m_movie->stop();

        m_baseSize = QSize();

        const QString location = System::getTempFilePath() + ".gif";

        using namespace std::placeholders;
        std::function<void(const QString &)> doneFun = std::bind(&AnimationGenerator::done, this, _1);
        auto doneCallback = make_cross_thread_function(this, doneFun);

        auto task = std::make_unique<GifGenerator>(data, location, doneCallback);
        m_executor->add(std::move(task));

        QProgressBar* progress = new QProgressBar;
        progress->setRange(0, 0);

        m_callback(progress, QString());
    }

    void scalePreview(double scale)
    {
        if (m_movie.get() != nullptr)
        {
            if (m_baseSize.isValid() == false)
                m_baseSize = m_movie->frameRect().size();

            const double scaleFactor = scale/100;
            QSize size = m_baseSize;
            size.rheight() *= scaleFactor;
            size.rwidth() *= scaleFactor;

            m_movie->setScaledSize(size);
        }
    }

    void done(const QString& location)
    {
        m_movie = std::make_unique<QMovie>(location);
        QLabel* label = new QLabel;

        label->setMovie(m_movie.get());
        m_movie->start();

        m_callback(label, location);
    }

    std::function<void(QWidget *, const QString &)> m_callback;
    std::unique_ptr<QMovie> m_movie;
    QSize m_baseSize;
    ITaskExecutor* m_executor;
};


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr>& photos, IExifReader* exifReader, ITaskExecutor* executor, QWidget *parent):
    QDialog(parent),
    m_model(),
    m_animationGenerator(),
    m_sortProxy(),
    m_representativeFile(),
    ui(new Ui::PhotosGroupingDialog),
    m_exifReader(exifReader),
    m_executor(executor)
{
    fillModel(photos);

    ui->setupUi(this);

    m_sortProxy.setSourceModel(&m_model);

    ui->photosView->setModel(&m_sortProxy);
    ui->photosView->setSortingEnabled(true);
    ui->photosView->sortByColumn(0, Qt::AscendingOrder);
    ui->photosView->resizeColumnsToContents();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);

    using namespace std::placeholders;
    auto callback = std::bind(&PhotosGroupingDialog::updatePreview, this, _1, _2);

    m_animationGenerator = std::make_unique<AnimationGenerator>(m_executor, callback);

    connect(ui->applyButton, &QPushButton::clicked, this, &PhotosGroupingDialog::makeAnimation);
    connect(ui->previewScaleSlider, &QSlider::sliderMoved, m_animationGenerator.get(), &AnimationGenerator::scalePreview);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


QString PhotosGroupingDialog::getRepresentative() const
{
    return m_representativeFile;
}


void PhotosGroupingDialog::updatePreview(QWidget* preview, const QString& location)
{
    if (preview != nullptr)
    {
        ui->resultPreview->setWidget(preview);
        m_representativeFile = location;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_representativeFile.isEmpty() == false);
}


void PhotosGroupingDialog::typeChanged()
{

}


void PhotosGroupingDialog::makeAnimation()
{
    AnimationGenerator::Data generator_data;

    generator_data.photos = getPhotos();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();

    m_animationGenerator->generatePreviewWidget(generator_data);
}


void PhotosGroupingDialog::fillModel(const std::vector<IPhotoInfo::Ptr>& photos)
{
    m_model.clear();

    for(const IPhotoInfo::Ptr& photo: photos)
    {
        const QString path = photo->getPath();
        const boost::any sequence_number = m_exifReader->get(path, IExifReader::TagType::SequenceNumber);

        const QString sequence_str = sequence_number.empty()? "-" : QString::number( boost::any_cast<int>(sequence_number));

        QStandardItem* pathItem = new QStandardItem(path);
        QStandardItem* sequenceItem = new QStandardItem(sequence_str);

        m_model.appendRow({pathItem, sequenceItem});
        m_model.setHeaderData(0, Qt::Horizontal, tr("photo path"));
        m_model.setHeaderData(1, Qt::Horizontal, tr("sequence number"));
    }
}


QStringList PhotosGroupingDialog::getPhotos() const
{
    QStringList result;

    for(int r = 0; r < m_sortProxy.rowCount(); r++)
    {
        const QModelIndex pathItemIdx = m_sortProxy.index(r, 0);
        const QVariant pathRaw = pathItemIdx.data(Qt::DisplayRole);
        const QString path = pathRaw.toString();
        const QFileInfo fileInfo(path);
        const QString absoluteFilePath = fileInfo.absoluteFilePath();

        result.append(absoluteFilePath);
    }

    return result;
}
