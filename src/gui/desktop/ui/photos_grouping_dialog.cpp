
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QMessageBox>
#include <QMovie>
#include <QPlainTextEdit>
#include <QProcess>

#include <core/configuration.hpp>
#include <core/constants.hpp>
#include <core/containers_utils.hpp>
#include <core/iexif_reader.hpp>
#include <core/down_cast.hpp>
#include <core/tags_utils.hpp>
#include <system/system.hpp>
#include <project_utils/project.hpp>
#include <project_utils/misc.hpp>

#include "ui_photos_grouping_dialog.h"

#include "utils/groups_manager.hpp"
#include "utils/grouppers/animation_generator.hpp"
#include "utils/grouppers/hdr_generator.hpp"
#include "utils/grouppers/collage_generator.hpp"
#include "widgets/media_preview.hpp"


namespace
{
    Group::Type comboboxToGroupType(int c)
    {
        if (c == 0)
            return Group::Animation;
        else if (c == 1)
            return Group::HDR;
        else if (c == 2)
            return Group::Collage;
        else
            return Group::Invalid;
    }

    int groupTypeToCombobox(Group::Type type)
    {
        switch(type)
        {
            case Group::Type::Invalid:   return -1;
            case Group::Type::Generic:   return -1;         // not expected
            case Group::Type::Animation: return 0;
            case Group::Type::HDR:       return 1;
            case Group::Type::Collage:   return 2;
        }

        return -1;
    }

    enum GenericForm
    {
        Collage = 0,
        OneOf   = 1,
    };
}


///////////////////////////////////////////////////////////////////////////////


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<Photo::Data>& photos,
                                           IExifReaderFactory& exifReader,
                                           ITaskExecutor& executor,
                                           IConfiguration& configuration,
                                           ILogger* logger,
                                           Group::Type type,
                                           QWidget *parent):
    QDialog(parent),
    m_tmpDir(System::createTmpDir("PGD_wd", System::Confidential)),
    m_sortProxy(),
    m_representativeFile(),
    m_photos(photos),
    m_representativeType(Group::Invalid),
    ui(new Ui::PhotosGroupingDialog),
    m_preview(new MediaPreview(this)),
    m_exifReaderFactory(exifReader),
    m_config(configuration),
    m_logger(logger),
    m_executor(executor),
    m_workInProgress(false)
{
    assert(photos.size() >= 2);

    fillModel(photos);

    ui->setupUi(this);
    ui->resultPreview->setWidget(m_preview);

    m_sortProxy.setSourceModel(&m_model);

    ui->photosList->setModel(&m_sortProxy);
    ui->photosList->setSortingEnabled(true);
    ui->photosList->sortByColumn(0, Qt::AscendingOrder);
    ui->photosList->resizeColumnsToContents();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    ui->generationProgressBar->reset();
    ui->speedSpinBox->setValue(calculateFPS());

    QIntValidator* heightValidator = new QIntValidator(this);
    heightValidator->setBottom(100);
    heightValidator->setTop(65536);
    ui->collageHeight->setValidator(heightValidator);

    if (type != Group::Type::Invalid)
        ui->groupingType->setCurrentIndex(groupTypeToCombobox(type));

    connect(ui->previewScaleSlider, &QSlider::sliderMoved, this, &PhotosGroupingDialog::scalePreview);
    connect(ui->previewButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewPressed);
    connect(ui->cancelButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewCancelPressed);
    connect(m_preview, &MediaPreview::scalableContentAvailable, [this](bool av)
    {
        ui->previewScaleSlider->setEnabled(av);                          // enable only when content available
        ui->previewScaleSlider->triggerAction(QSlider::SliderToMaximum); // reset slider's position
    });
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{
    delete ui;
}


QString PhotosGroupingDialog::getRepresentative() const
{
    return m_representativeFile;
}


Group::Type PhotosGroupingDialog::groupType() const
{
    return m_representativeType;
}


const std::vector<Photo::Data>& PhotosGroupingDialog::photos() const
{
    return m_photos;
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
    m_representativeType = comboboxToGroupType(ui->optionsWidget->currentIndex());

    m_workInProgress = false;

    if (m_representativeFile.isEmpty() == false)
        m_preview->setMedia(m_representativeFile);

    switchUiToGenerationFinished();
}


void PhotosGroupingDialog::generationCanceled()
{
    generationDone(QString());
}


void PhotosGroupingDialog::generationError(const QString& info, const QStringList& output)
{
    generationDone(QString());

    QDialog errorReporter(this);
    errorReporter.setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(&errorReporter);

    QHBoxLayout* infoLayout = new QHBoxLayout;

    QLabel* infoIcon = new QLabel;
    infoIcon->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxCritical));
    infoLayout->addWidget(infoIcon);
    infoLayout->addWidget(new QLabel(info));

    layout->addLayout(infoLayout);

    if (output.isEmpty() == false)
    {
        QPlainTextEdit* outputContainer = new QPlainTextEdit(&errorReporter);
        outputContainer->setReadOnly(true);
        outputContainer->setPlainText(output.join("\n"));
        outputContainer->setWordWrapMode(QTextOption::NoWrap);
        layout->addWidget(outputContainer);
    }

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &errorReporter);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &errorReporter, &QDialog::accept);

    errorReporter.exec();
}


void PhotosGroupingDialog::refreshDialogButtons()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_representativeFile.isEmpty() == false);
}


void PhotosGroupingDialog::previewPressed()
{
    switchUiToGeneration();

    const int tool_page = ui->optionsWidget->currentIndex();
    auto type = comboboxToGroupType(tool_page);

    switch(type)
    {
        case Group::Animation:
            makeAnimation();
            break;

        case Group::HDR:
            makeHDR();
            break;

        case Group::Collage:
            makeCollage();
            break;

        case Group::Invalid:
        case Group::Generic:
            assert(!"I should not be here");
            break;
    }
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

    generator_data.storage = m_tmpDir->path();
    generator_data.alignImageStackPath = m_config.getEntry(ExternalToolsConfigKeys::aisPath).toString();
    generator_data.magickPath = m_config.getEntry(ExternalToolsConfigKeys::magickPath).toString();
    generator_data.photos = getPhotos();
    generator_data.format = ui->formatComboBox->currentText();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();
    generator_data.delay = ui->delaySpinBox->value();
    generator_data.stabilize = ui->stabilizationCheckBox->isChecked();

    auto animation_task = std::make_unique<AnimationGenerator>(generator_data, m_logger, m_exifReaderFactory);

    startTask(std::move(animation_task));
}


void PhotosGroupingDialog::makeHDR()
{
    HDRGenerator::Data generator_data;

    generator_data.storage = m_tmpDir->path();
    generator_data.alignImageStackPath = m_config.getEntry(ExternalToolsConfigKeys::aisPath).toString();
    generator_data.magickPath = m_config.getEntry(ExternalToolsConfigKeys::magickPath).toString();
    generator_data.photos = getPhotos();

    auto hdr_task = std::make_unique<HDRGenerator>(generator_data, m_logger, m_exifReaderFactory);

    startTask(std::move(hdr_task));
}


void PhotosGroupingDialog::makeCollage()
{
    CollageGenerator generator(m_exifReaderFactory.get());
    const int height = ui->collageHeight->text().toInt();
    const QImage collage = generator.generateCollage(getPhotos(), height);

    if (collage.isNull())
        generationError(tr("Error during collage generation. Possibly too many images, or height to small or too big."), {});
    else
    {
        const QString collagePath = System::getUniqueFileName(m_tmpDir->path(), "jpeg");

        collage.save(collagePath);
        generationDone(collagePath);
    }
}


void PhotosGroupingDialog::fillModel(const std::vector<Photo::Data>& photos)
{
    m_model.clear();

    IExifReader& exif = m_exifReaderFactory.get();

    const QRegularExpression burstRE(".*BURST([0-9]+).*");

    for(const Photo::Data& photo: photos)
    {
        const QString& path = photo.path;
        const std::optional<std::any> sequence_number = exif.get(path, IExifReader::TagType::SequenceNumber);
        const std::optional<std::any> exposure_number = exif.get(path, IExifReader::TagType::Exposure);

        const QRegularExpressionMatch burstMatch = burstRE.match(path);
        const QString burst_str = burstMatch.hasMatch()? burstMatch.captured(1) : "-";
        const QString sequence_str = sequence_number.has_value()? QString::number( std::any_cast<int>(*sequence_number)): burst_str;
        const QString exposure_str = exposure_number.has_value()? QString::number( std::any_cast<float>(*exposure_number)): "-";

        QStandardItem* pathItem = new QStandardItem(path);
        QStandardItem* sequenceItem = new QStandardItem(sequence_str);
        QStandardItem* exposureItem = new QStandardItem(exposure_str);

        m_model.appendRow({pathItem, sequenceItem, exposureItem});
        m_model.setHeaderData(0, Qt::Horizontal, tr("photo path"));
        m_model.setHeaderData(1, Qt::Horizontal, tr("sequence number"));
        m_model.setHeaderData(2, Qt::Horizontal, tr("exposure (EV)"));
    }
}


double PhotosGroupingDialog::calculateFPS() const
{
    std::set<std::chrono::milliseconds> timestamps;

    for(const auto& photo: m_photos)
    {
        const auto timestamp = Tag::timestamp(photo.tags);
        timestamps.insert(timestamp);
    }

    const auto diff = back(timestamps) - front(timestamps);

    return diff.count() > 0? (m_photos.size() * 1000.0 / diff.count()): 10.0;
}


void PhotosGroupingDialog::startTask(std::unique_ptr<GeneratorUtils::BreakableTask> task)
{
    connect(this, &PhotosGroupingDialog::cancel, task.get(), &AnimationGenerator::cancel);
    connect(task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
    connect(task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
    connect(task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);
    connect(task.get(), &AnimationGenerator::canceled,  this, &PhotosGroupingDialog::generationCanceled);
    connect(task.get(), &AnimationGenerator::error,     this, &PhotosGroupingDialog::generationError);

    m_executor.add(std::move(task));
}


void PhotosGroupingDialog::switchUiToGeneration()
{
    ui->previewButtons->setCurrentIndex(1);

    ui->generationProgressBar->setEnabled(true);
    ui->optionsWidget->setEnabled(false);
    m_preview->clean();
    m_workInProgress = true;
    m_representativeFile.clear();

    refreshDialogButtons();
}


void PhotosGroupingDialog::switchUiToGenerationFinished()
{
    ui->previewButtons->setCurrentIndex(0);

    ui->generationProgressBar->reset();
    ui->generationProgressBar->setDisabled(true);
    ui->operationName->setText("");
    ui->optionsWidget->setEnabled(true);

    refreshDialogButtons();
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
    const int scale = ui->previewScaleSlider->value();
    const double scaleFactor = scale/100.0;

    m_preview->scale(scaleFactor);
}
