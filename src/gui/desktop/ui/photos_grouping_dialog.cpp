
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QMovie>
#include <QProcess>

#include <core/iexif_reader.hpp>
#include <core/down_cast.hpp>

#include "ui_photos_grouping_dialog.h"

#include "utils/grouppers/animation_generator.hpp"


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<IPhotoInfo::Ptr>& photos, IExifReader* exifReader, ITaskExecutor* executor, QWidget *parent):
    QDialog(parent),
    m_model(),
    m_movie(),
    m_sortProxy(),
    m_representativeFile(),
    ui(new Ui::PhotosGroupingDialog),
    m_exifReader(exifReader),
    m_executor(executor)
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

    connect(ui->applyButton, &QPushButton::clicked, this, &PhotosGroupingDialog::makeAnimation);
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


QString PhotosGroupingDialog::getRepresentative() const
{
    return m_representativeFile;
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

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_representativeFile.isEmpty() == false);

    m_movie = std::make_unique<QMovie>(location);
    QLabel* label = new QLabel;

    label->setMovie(m_movie.get());
    m_movie->start();

    ui->resultPreview->setWidget(label);

    ui->generationProgressBar->reset();
    ui->generationProgressBar->setDisabled(true);
    ui->operationName->setText("");
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
    generator_data.delay = ui->delaySpinBox->value();
    generator_data.stabilize = ui->stabilizationCheckBox->isChecked();

    auto animation_task = std::make_unique<AnimationGenerator>(generator_data);

    connect(animation_task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
    connect(animation_task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
    connect(animation_task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);

    m_executor->add(std::move(animation_task));
    ui->generationProgressBar->setEnabled(true);
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
