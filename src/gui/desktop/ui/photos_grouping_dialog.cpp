
#include "photos_grouping_dialog.hpp"

#include <QFileInfo>
#include <QMessageBox>
#include <QMovie>
#include <QPlainTextEdit>
#include <QProcess>

#include <core/containers_utils.hpp>
#include <core/iexif_reader.hpp>
#include <core/tags_utils.hpp>
#include <project_utils/project.hpp>
#include <project_utils/misc.hpp>

#include "ui_photos_grouping_dialog.h"

#include "utils/groups_manager.hpp"
#include "utils/grouppers/animation_generator.hpp"
#include "utils/grouppers/hdr_generator.hpp"
#include "utils/grouppers/collage_generator.hpp"
#include "widgets/media_preview.hpp"

import system;

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

struct PhotosGroupingDialog::Impl
{
    using ExplicitDelta = PhotosGroupingDialog::ExplicitDelta;

    Impl(const std::vector<ExplicitDelta>& photos_,
         IExifReaderFactory& exifReaderFactory_,
         IConfiguration& config_,
         ILogger* logger_,
         ITaskExecutor& executor_,
         QWidget* parent):
        model(),
        tmpDir(System::createTmpDir("PGD_wd", QFlags<System::TmpOption>{System::Confidential, System::BigFiles})),
        sortProxy(),
        representativeFile(),
        photos(photos_),
        representativeType(Group::Invalid),
        ui(),
        preview(new MediaPreview(parent)),
        exifReaderFactory(exifReaderFactory_),
        config(config_),
        logger(logger_),
        executor(executor_),
        workInProgress(false)
    {
    }

    QStandardItemModel model;
    std::shared_ptr<ITmpDir> tmpDir;
    SortingProxy sortProxy;
    QString representativeFile;
    std::vector<ExplicitDelta> photos;
    Group::Type representativeType;
    Ui::PhotosGroupingDialog ui;
    MediaPreview* preview;
    IExifReaderFactory& exifReaderFactory;
    IConfiguration& config;
    ILogger* logger;
    ITaskExecutor& executor;
    bool workInProgress;
};


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<ExplicitDelta>& photos,
                                           IExifReaderFactory& exifReader,
                                           ITaskExecutor& executor,
                                           IConfiguration& configuration,
                                           ILogger* logger,
                                           Group::Type type,
                                           QWidget *parent):
    QDialog(parent),
    m_impl(std::make_unique<Impl>(photos, exifReader, configuration, logger, executor, this))
{
    assert(photos.size() >= 2);

    fillModel(photos);

    m_impl->ui.setupUi(this);
    m_impl->ui.resultPreview->setWidget(m_impl->preview);

    m_impl->sortProxy.setSourceModel(&m_impl->model);

    m_impl->ui.photosList->setModel(&m_impl->sortProxy);
    m_impl->ui.photosList->setSortingEnabled(true);
    m_impl->ui.photosList->sortByColumn(0, Qt::AscendingOrder);
    m_impl->ui.photosList->resizeColumnsToContents();
    m_impl->ui.buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    m_impl->ui.generationProgressBar->reset();
    m_impl->ui.speedSpinBox->setValue(calculateFPS());

    QIntValidator* heightValidator = new QIntValidator(this);
    heightValidator->setBottom(100);
    heightValidator->setTop(65536);
    m_impl->ui.collageHeight->setValidator(heightValidator);

    if (type != Group::Type::Invalid)
        m_impl->ui.groupingType->setCurrentIndex(groupTypeToCombobox(type));

    connect(m_impl->ui.previewScaleSlider, &QSlider::sliderMoved, this, &PhotosGroupingDialog::scalePreview);
    connect(m_impl->ui.previewButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewPressed);
    connect(m_impl->ui.cancelButton, &QPushButton::clicked, this, &PhotosGroupingDialog::previewCancelPressed);
    connect(m_impl->preview, &MediaPreview::scalableContentAvailable, [this](bool av)
    {
        m_impl->ui.previewScaleSlider->setEnabled(av);                          // enable only when content available
        m_impl->ui.previewScaleSlider->triggerAction(QSlider::SliderToMaximum); // reset slider's position
    });
}


PhotosGroupingDialog::~PhotosGroupingDialog()
{

}


QString PhotosGroupingDialog::getRepresentative() const
{
    return m_impl->representativeFile;
}


Group::Type PhotosGroupingDialog::groupType() const
{
    return m_impl->representativeType;
}


void PhotosGroupingDialog::reject()
{
    if (m_impl->workInProgress)
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
    m_impl->ui.generationProgressBar->setValue(0);
    m_impl->ui.operationName->setText(title);
}


void PhotosGroupingDialog::generationProgress(int v)
{
    if (v == -1)
        m_impl->ui.generationProgressBar->setMaximum(0);
    else
    {
        m_impl->ui.generationProgressBar->setMaximum(100);
        m_impl->ui.generationProgressBar->setValue(v);
    }
}


void PhotosGroupingDialog::generationDone(const QString& location)
{
    m_impl->representativeFile = location;
    m_impl->representativeType = comboboxToGroupType(m_impl->ui.optionsWidget->currentIndex());

    m_impl->workInProgress = false;

    if (m_impl->representativeFile.isEmpty() == false)
        m_impl->preview->setMedia(m_impl->representativeFile);

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
    m_impl->ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_impl->representativeFile.isEmpty() == false);
}


void PhotosGroupingDialog::previewPressed()
{
    switchUiToGeneration();

    const int tool_page = m_impl->ui.optionsWidget->currentIndex();
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
        m_impl->ui.previewButtons->setCurrentIndex(0);
        emit cancel();
    }
}


void PhotosGroupingDialog::makeAnimation()
{
    AnimationGenerator::Data generator_data;

    generator_data.storage = m_impl->tmpDir->path();
    generator_data.photos = getPhotos();
    generator_data.fps = m_impl->ui.speedSpinBox->value();
    generator_data.scale = m_impl->ui.scaleSpinBox->value();
    generator_data.delay = m_impl->ui.delaySpinBox->value();
    generator_data.stabilize = m_impl->ui.stabilizationCheckBox->isChecked();

    auto animation_task = std::make_unique<AnimationGenerator>(generator_data, m_impl->logger, m_impl->exifReaderFactory);

    startTask(std::move(animation_task));
}


void PhotosGroupingDialog::makeHDR()
{
    HDRGenerator::Data generator_data;

    generator_data.storage = m_impl->tmpDir->path();
    generator_data.photos = getPhotos();

    auto hdr_task = std::make_unique<HDRGenerator>(generator_data, m_impl->logger, m_impl->exifReaderFactory);

    startTask(std::move(hdr_task));
}


void PhotosGroupingDialog::makeCollage()
{
    CollageGenerator generator(m_impl->exifReaderFactory.get());
    const int height = m_impl->ui.collageHeight->text().toInt();
    const QImage collage = generator.generateCollage(getPhotos(), height);

    if (collage.isNull())
        generationError(tr("Error during collage generation. Possibly too many images, or height to small or too big."), {});
    else
    {
        const QString collagePath = System::getUniqueFileName(m_impl->tmpDir->path(), "jpeg");

        collage.save(collagePath);
        generationDone(collagePath);
    }
}


void PhotosGroupingDialog::fillModel(const std::vector<ExplicitDelta>& photos)
{
    m_impl->model.clear();

    IExifReader& exif = m_impl->exifReaderFactory.get();

    const QRegularExpression burstRE(".*BURST([0-9]+).*");

    for(const auto& photo: photos)
    {
        const QString& path = photo.get<Photo::Field::Path>();
        const std::optional<std::any> sequence_number = exif.get(path, IExifReader::TagType::SequenceNumber);
        const std::optional<std::any> exposure_number = exif.get(path, IExifReader::TagType::Exposure);

        const QRegularExpressionMatch burstMatch = burstRE.match(path);
        const QString burst_str = burstMatch.hasMatch()? burstMatch.captured(1) : "-";
        const QString sequence_str = sequence_number.has_value()? QString::number( std::any_cast<int>(*sequence_number)): burst_str;
        const QString exposure_str = exposure_number.has_value()? QString::number( std::any_cast<float>(*exposure_number)): "-";

        QStandardItem* pathItem = new QStandardItem(path);
        QStandardItem* sequenceItem = new QStandardItem(sequence_str);
        QStandardItem* exposureItem = new QStandardItem(exposure_str);

        m_impl->model.appendRow({pathItem, sequenceItem, exposureItem});
        m_impl->model.setHeaderData(0, Qt::Horizontal, tr("photo path"));
        m_impl->model.setHeaderData(1, Qt::Horizontal, tr("sequence number"));
        m_impl->model.setHeaderData(2, Qt::Horizontal, tr("exposure (EV)"));
    }
}


double PhotosGroupingDialog::calculateFPS() const
{
    std::set<std::chrono::milliseconds> timestamps;

    for(const auto& photo: m_impl->photos)
    {
        const auto timestamp = Tag::timestamp(photo.get<Photo::Field::Tags>());
        timestamps.insert(timestamp);
    }

    const auto diff = back(timestamps) - front(timestamps);

    return diff.count() > 0? (static_cast<double>(m_impl->photos.size()) * 1000.0 / static_cast<double>(diff.count())): 10.0;
}


void PhotosGroupingDialog::startTask(std::unique_ptr<GeneratorUtils::BreakableTask> task)
{
    connect(this, &PhotosGroupingDialog::cancel, task.get(), &AnimationGenerator::cancel);
    connect(task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
    connect(task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
    connect(task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);
    connect(task.get(), &AnimationGenerator::canceled,  this, &PhotosGroupingDialog::generationCanceled);
    connect(task.get(), &AnimationGenerator::error,     this, &PhotosGroupingDialog::generationError);

    m_impl->executor.add(std::move(task));
}


void PhotosGroupingDialog::switchUiToGeneration()
{
    m_impl->ui.previewButtons->setCurrentIndex(1);

    m_impl->ui.generationProgressBar->setEnabled(true);
    m_impl->ui.optionsWidget->setEnabled(false);
    m_impl->preview->clean();
    m_impl->workInProgress = true;
    m_impl->representativeFile.clear();

    refreshDialogButtons();
}


void PhotosGroupingDialog::switchUiToGenerationFinished()
{
    m_impl->ui.previewButtons->setCurrentIndex(0);

    m_impl->ui.generationProgressBar->reset();
    m_impl->ui.generationProgressBar->setDisabled(true);
    m_impl->ui.operationName->setText("");
    m_impl->ui.optionsWidget->setEnabled(true);

    refreshDialogButtons();
}


QStringList PhotosGroupingDialog::getPhotos() const
{
    QStringList result;

    for(int r = 0; r < m_impl->sortProxy.rowCount(); r++)
    {
        const QModelIndex pathItemIdx = m_impl->sortProxy.index(r, 0);
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
    const int scale = m_impl->ui.previewScaleSlider->value();
    const double scaleFactor = scale/100.0;

    m_impl->preview->scale(scaleFactor);
}
