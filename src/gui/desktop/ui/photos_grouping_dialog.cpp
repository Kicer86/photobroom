
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QMessageBox>
#include <QMovie>
#include <QProcess>

#include <core/configuration.hpp>
#include <core/constants.hpp>
#include <core/iexif_reader.hpp>
#include <core/down_cast.hpp>

#include "ui_photos_grouping_dialog.h"

#include "utils/grouppers/animation_generator.hpp"


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<Photo::Data>& photos,
                                           IExifReader* exifReader,
                                           ITaskExecutor* executor,
                                           IConfiguration* configuration,
                                           ILogger* logger,
                                           QWidget *parent):
    QDialog(parent),
    m_model(),
    m_movie(),
    m_sortProxy(),
    m_representativeFile(),
    ui(new Ui::PhotosGroupingDialog),
    m_exifReader(exifReader),
    m_config(configuration),
    m_logger(logger),
    m_executor(executor),
    m_workInProgress(false)
{
    assert(photos.size() >= 2);

    fillModel(photos);

    ui->setupUi(this);

    m_sortProxy.setSourceModel(&m_model);

    ui->photosList->setModel(&m_sortProxy);
    ui->photosList->setSortingEnabled(true);
    ui->photosList->sortByColumn(0, Qt::AscendingOrder);
    ui->photosList->resizeColumnsToContents();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->generationProgressBar->reset();

    connect(ui->previewButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewPressed);
    connect(ui->previewButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewCancelPressed);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


QString PhotosGroupingDialog::getRepresentative() const
{
    return m_representativeFile;
}


void PhotosGroupingDialog::reject()
{
    if (m_workInProgress)
    {
        const QMessageBox::StandardButton result = QMessageBox::question(this, tr("Cancel operation?"), tr("Do you really want to stop current work and quit?"));

        if (result == QMessageBox::StandardButton::Yes)
        {
            emit cancel();
            QDialog::reject();
        }
    }
    else
        QDialog::reject();
}


void PhotosGroupingDialog::generationTitle(const QString& title)
{
    ui->generationProgressBar->setValue(0);
    ui->operationName->setText(title);
}


void PhotosGroupingDialog::generationProgress(int v)
{
    if (v == -1)
        ui->generationProgressBar->setMaximum(0);
    else
    {
        ui->generationProgressBar->setMaximum(100);
        ui->generationProgressBar->setValue(v);
    }
}


void PhotosGroupingDialog::generationDone(const QString& location)
{
    m_representativeFile = location;
    m_workInProgress = false;

    if (m_representativeFile.isEmpty() == false)
    {
        m_movie = std::make_unique<QMovie>(location);
        QLabel* label = new QLabel;

        label->setMovie(m_movie.get());
        m_movie->start();

        ui->resultPreview->setWidget(label);
    }

    ui->generationProgressBar->reset();
    ui->generationProgressBar->setDisabled(true);
    ui->operationName->setText("");
    ui->animationOptions->setEnabled(true);
    ui->previewButtons->setCurrentIndex(0);

    refreshDialogButtons();
}


void PhotosGroupingDialog::refreshDialogButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_representativeFile.isEmpty() == false);
}


void PhotosGroupingDialog::typeChanged()
{

}


void PhotosGroupingDialog::previewPressed()
{
    makeAnimation();
    ui->previewButtons->setCurrentIndex(1);
}


void PhotosGroupingDialog::previewCancelPressed()
{
    const QMessageBox::StandardButton result = QMessageBox::question(this, tr("Cancel operation?"), tr("Do you really want to stop current work?"));

    if (result == QMessageBox::StandardButton::Yes)
    {
        ui->previewButtons->setCurrentIndex(0);
        emit cancel();
    }
}


void PhotosGroupingDialog::makeAnimation()
{
    AnimationGenerator::Data generator_data;

    generator_data.alignImageStackPath = m_config->getEntry(ExternalToolsConfigKeys::aisPath).toString();
    generator_data.convertPath = m_config->getEntry(ExternalToolsConfigKeys::convertPath).toString();
    generator_data.photos = getPhotos();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();
    generator_data.delay = ui->delaySpinBox->value();
    generator_data.stabilize = ui->stabilizationCheckBox->isChecked();

    // make sure we have all neccessary data
    if (generator_data.convertPath.isEmpty())
        QMessageBox::critical(this,
                              tr("Missing tool"),
                              tr("'convert' tool is neccessary for this operation.\n"
                                 "Please go to settings and setup path to 'convert' executable.\n\n"
                                 "'convert' is a tool which is a part of ImageMagick.\n"
                                 "Visit https://www.imagemagick.org/ for downloads."));

    else if(generator_data.stabilize && generator_data.alignImageStackPath.isEmpty())
        QMessageBox::critical(this,
                              tr("Missing tool"),
                              tr("'align_image_stack' tool is neccessary to stabilize animation.\n"
                                 "Please go to settings and setup path to 'align_image_stack' executable.\n\n"
                                 "'align_image_stack' is a tool which is a part of Hugin.\n"
                                 "Visit http://hugin.sourceforge.net/ for downloads."));
    else
    {
        auto animation_task = std::make_unique<AnimationGenerator>(generator_data, m_logger);

        connect(this, &PhotosGroupingDialog::cancel, animation_task.get(), &AnimationGenerator::cancel);
        connect(ui->previewScaleSlider, &QSlider::sliderMoved,        this, &PhotosGroupingDialog::scalePreview);
        connect(animation_task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
        connect(animation_task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
        connect(animation_task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);

        m_executor->add(std::move(animation_task));
        ui->generationProgressBar->setEnabled(true);
        ui->animationOptions->setEnabled(false);
        ui->resultPreview->setWidget(new QWidget);
        m_workInProgress = true;
        m_representativeFile.clear();

        refreshDialogButtons();
    }
}


void PhotosGroupingDialog::fillModel(const std::vector<Photo::Data>& photos)
{
    m_model.clear();

    for(const Photo::Data& photo: photos)
    {
        const QString& path = photo.path;
        const std::any sequence_number = m_exifReader->get(path, IExifReader::TagType::SequenceNumber);

        const QString sequence_str = sequence_number.has_value()? QString::number( std::any_cast<int>(sequence_number)): "-";

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


void PhotosGroupingDialog::scalePreview()
{
    if (m_movie.get() != nullptr)
    {
        if (m_baseSize.isValid() == false)
            m_baseSize = m_movie->frameRect().size();

        const int scale = ui->previewScaleSlider->value();

        const double scaleFactor = scale/100.0;
        QSizeF size = m_baseSize;
        size.rheight() *= scaleFactor;
        size.rwidth() *= scaleFactor;

        m_movie->setScaledSize(size.toSize());
    }
}
