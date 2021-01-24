
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
#include "widgets/media_preview.hpp"


namespace
{
    Group::Type comboboxToGroupType(int c)
    {
        if (c == 0)
            return Group::Animation;
        else if (c == 1)
            return Group::HDR;
        else
            return Group::Invalid;
    }

    int groupTypeTocombobox(Group::Type type)
    {
        switch(type)
        {
            case Group::Type::Invalid:   return -1;
            case Group::Type::Animation: return 0;
            case Group::Type::HDR:       return 1;
        }

        return -1;
    }
}

///////////////////////////////////////////////////////////////////////////////


namespace PhotosGroupingDialogUtils
{
    void createGroup(PhotosGroupingDialog* dialog, Project* project, Database::IDatabase* db)
    {
        const auto& photos = dialog->photos();
        const QString photo = dialog->getRepresentative();
        const Group::Type type = dialog->groupType();

        std::vector<Photo::Id> photos_ids;
        for(std::size_t i = 0; i < photos.size(); i++)
            photos_ids.push_back(photos[i].id);

        const QString internalPath = copyFileToPrivateMediaLocation(project->getProjectInfo(), photo);
        const QString internalPathDecorated = project->makePathRelative(internalPath);

        GroupsManager::group(db, photos_ids, internalPathDecorated, type);
    }
}


///////////////////////////////////////////////////////////////////////////////


PhotosGroupingDialog::PhotosGroupingDialog(const std::vector<Photo::Data>& photos,
                                           IExifReaderFactory* exifReader,
                                           ITaskExecutor* executor,
                                           IConfiguration* configuration,
                                           ILogger* logger,
                                           Group::Type type,
                                           QWidget *parent):
    QDialog(parent),
    m_model(),
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

    if (type != Group::Type::Invalid)
        ui->groupingType->setCurrentIndex(groupTypeTocombobox(type));

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

    ui->generationProgressBar->reset();
    ui->generationProgressBar->setDisabled(true);
    ui->operationName->setText("");
    ui->animationOptions->setEnabled(true);
    ui->previewButtons->setCurrentIndex(0);

    refreshDialogButtons();
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
    layout->addWidget(new QLabel(info));

    QPlainTextEdit* outputContainer = new QPlainTextEdit(&errorReporter);
    outputContainer->setReadOnly(true);
    outputContainer->setPlainText(output.join("\n"));
    outputContainer->setWordWrapMode(QTextOption::NoWrap);
    layout->addWidget(outputContainer);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &errorReporter);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &errorReporter, &QDialog::accept);

    errorReporter.exec();
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

        case Group::Generic:
            assert(!"implement");
            break;

        case Group::Invalid:
            assert(!"I should not be here");
            break;
    }

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

    generator_data.storage = m_tmpDir->path();
    generator_data.alignImageStackPath = m_config->getEntry(ExternalToolsConfigKeys::aisPath).toString();
    generator_data.magickPath = m_config->getEntry(ExternalToolsConfigKeys::magickPath).toString();
    generator_data.photos = getPhotos();
    generator_data.format = ui->formatComboBox->currentText();
    generator_data.fps = ui->speedSpinBox->value();
    generator_data.scale = ui->scaleSpinBox->value();
    generator_data.delay = ui->delaySpinBox->value();
    generator_data.stabilize = ui->stabilizationCheckBox->isChecked();

    // make sure we have all neccessary data
    if (generator_data.magickPath.isEmpty())
        QMessageBox::critical(this,
                              tr("Missing tool"),
                              tr("'magick' tool is neccessary for this operation.\n"
                                 "Please go to settings and setup path to 'magick' executable.\n\n"
                                 "'magick' is a tool provided by ImageMagick.\n"
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
        auto animation_task = std::make_unique<AnimationGenerator>(generator_data, m_logger, m_exifReaderFactory);

        connect(this, &PhotosGroupingDialog::cancel, animation_task.get(), &AnimationGenerator::cancel);
        connect(ui->previewScaleSlider, &QSlider::sliderMoved,        this, &PhotosGroupingDialog::scalePreview);
        connect(animation_task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
        connect(animation_task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
        connect(animation_task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);
        connect(animation_task.get(), &AnimationGenerator::canceled,  this, &PhotosGroupingDialog::generationCanceled);
        connect(animation_task.get(), &AnimationGenerator::error,     this, &PhotosGroupingDialog::generationError);

        m_executor->add(std::move(animation_task));
        ui->generationProgressBar->setEnabled(true);
        ui->animationOptions->setEnabled(false);
        m_preview->clean();
        m_workInProgress = true;
        m_representativeFile.clear();

        refreshDialogButtons();
    }
}


void PhotosGroupingDialog::makeHDR()
{
    HDRGenerator::Data generator_data;

    generator_data.storage = m_tmpDir->path();
    generator_data.alignImageStackPath = m_config->getEntry(ExternalToolsConfigKeys::aisPath).toString();
    generator_data.magickPath = m_config->getEntry(ExternalToolsConfigKeys::magickPath).toString();
    generator_data.photos = getPhotos();

    // make sure we have all neccessary data
    if (generator_data.magickPath.isEmpty())
        QMessageBox::critical(this,
                              tr("Missing tool"),
                              tr("'magick' tool is neccessary for this operation.\n"
                                 "Please go to settings and setup path to 'convert' executable.\n\n"
                                 "'magick' is a tool provided by ImageMagick.\n"
                                 "Visit https://www.imagemagick.org/ for downloads."));

    else if(generator_data.alignImageStackPath.isEmpty())
        QMessageBox::critical(this,
                              tr("Missing tool"),
                              tr("'align_image_stack' tool is neccessary to generate HDR image.\n"
                                 "Please go to settings and setup path to 'align_image_stack' executable.\n\n"
                                 "'align_image_stack' is a tool which is a part of Hugin.\n"
                                 "Visit http://hugin.sourceforge.net/ for downloads."));
    else
    {
        auto hdr_task = std::make_unique<HDRGenerator>(generator_data, m_logger, m_exifReaderFactory);

        connect(this, &PhotosGroupingDialog::cancel, hdr_task.get(), &AnimationGenerator::cancel);
        connect(ui->previewScaleSlider, &QSlider::sliderMoved,  this, &PhotosGroupingDialog::scalePreview);
        connect(hdr_task.get(), &AnimationGenerator::operation, this, &PhotosGroupingDialog::generationTitle);
        connect(hdr_task.get(), &AnimationGenerator::progress,  this, &PhotosGroupingDialog::generationProgress);
        connect(hdr_task.get(), &AnimationGenerator::finished,  this, &PhotosGroupingDialog::generationDone);
        connect(hdr_task.get(), &AnimationGenerator::canceled,  this, &PhotosGroupingDialog::generationCanceled);
        connect(hdr_task.get(), &AnimationGenerator::error,     this, &PhotosGroupingDialog::generationError);

        m_executor->add(std::move(hdr_task));
        ui->generationProgressBar->setEnabled(true);
        ui->animationOptions->setEnabled(false);
        m_preview->clean();
        m_workInProgress = true;
        m_representativeFile.clear();

        refreshDialogButtons();
    }
}


void PhotosGroupingDialog::fillModel(const std::vector<Photo::Data>& photos)
{
    m_model.clear();

    IExifReader* exif = m_exifReaderFactory->get();

    for(const Photo::Data& photo: photos)
    {
        const QString& path = photo.path;
        const std::optional<std::any> sequence_number = exif->get(path, IExifReader::TagType::SequenceNumber);

        const QString sequence_str = sequence_number.has_value()? QString::number( std::any_cast<int>(*sequence_number)): "-";

        QStandardItem* pathItem = new QStandardItem(path);
        QStandardItem* sequenceItem = new QStandardItem(sequence_str);

        m_model.appendRow({pathItem, sequenceItem});
        m_model.setHeaderData(0, Qt::Horizontal, tr("photo path"));
        m_model.setHeaderData(1, Qt::Horizontal, tr("sequence number"));
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
