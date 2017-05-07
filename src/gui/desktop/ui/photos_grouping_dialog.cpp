
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QProcess>

#include <core/iexif_reader.hpp>

#include "ui_photos_grouping_dialog.h"

#include "utils/grouppers/animation_generator.hpp"


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
    assert(photos.size() >= 2);

    fillModel(photos);

    ui->setupUi(this);

    m_sortProxy.setSourceModel(&m_model);

    ui->photosList->setModel(&m_sortProxy);
    ui->photosList->setSortingEnabled(true);
    ui->photosList->sortByColumn(0, Qt::AscendingOrder);
    ui->photosList->resizeColumnsToContents();
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
    generator_data.delay = ui->delaySpinBox->value();
    generator_data.stabilize = ui->stabilizationCheckBox->isChecked();

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
