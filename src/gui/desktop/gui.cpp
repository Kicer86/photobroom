
#include "gui.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QImageReader>
#include <QStandardPaths>
#include <QTranslator>

#include <core/constants.hpp>
#include <core/iconfiguration.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger.hpp>
#include <core/itask_executor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/thumbnail_generator.hpp>
#include <core/thumbnail_manager.hpp>
#include <system/filesystem.hpp>

#ifdef UPDATER_ENABLED
#include <updater/updater.hpp>
#endif

#include "ui/mainwindow.hpp"
#include "features.hpp"


namespace
{
    struct ImagesDetector: IFeatureDetector
    {
        ImagesDetector(std::unique_ptr<ILogger> logger): m_logger(std::move(logger)) {}

        QStringList detect() override
        {
            QStringList features;

            QList<QByteArray> images = QImageReader::supportedImageFormats();

            for(const QByteArray image: images)
            {
                const std::string msg = std::string("Qt supports ") + image.data() + " file format";

                m_logger->debug(msg);

                if (image == "mng")
                    features.append(gui::features::MngFile);
            }

            return features;
        }

        std::unique_ptr<ILogger> m_logger;
    };
}


Gui::Gui(int& argc, char **argv):
    m_app(new QApplication(argc, argv)),
    m_prjManager(nullptr),
    m_pluginLoader(nullptr),
    m_coreFactory(nullptr)
{

}


Gui::~Gui()
{

}


QCoreApplication* Gui::getApp()
{
    return m_app.get();
}


void Gui::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void Gui::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
}


void Gui::set(ICoreFactoryAccessor* coreFactory)
{
    m_coreFactory = coreFactory;
}


void Gui::run()
{
    assert(m_prjManager != nullptr);
    assert(m_pluginLoader != nullptr);
    assert(m_coreFactory != nullptr);

#ifdef GUI_STATIC
    // see: http://doc.qt.io/qt-5/resources.html
    Q_INIT_RESOURCE(images);
#endif

    // On Windows, add extra location for Qt plugins
#ifdef OS_WIN
    m_app->addLibraryPath(FileSystem().getLibrariesPath());
#endif

    ILoggerFactory* loggerFactory = m_coreFactory->getLoggerFactory();

    auto gui_logger = loggerFactory->get("Gui");
    auto photos_manager_logger = loggerFactory->get("Photos manager");

    const QString tr_path = FileSystem().getTranslationsPath();
    InfoStream( gui_logger.get()) << QString("Searching for translations in: %1").arg(tr_path);

    // translations
    const QLocale locale;

    const QString info = QString("System language: %1").arg(locale.name());
    gui_logger->debug(info.toStdString());

    const auto uiLangs = locale.uiLanguages();
    const QString uiLangsStr = uiLangs.join(", ");
    const QString language_details = QString("List of UI langauges: %1").arg(uiLangsStr);
    gui_logger->debug(language_details.toStdString());

    bool translations_status = false;
    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("photo_broom"), QLatin1String("_"), tr_path))
        translations_status = QCoreApplication::installTranslator(&translator);

    if (translations_status)
        gui_logger->log(ILogger::Severity::Info, "Translations loaded successfully.");
    else
        gui_logger->log(ILogger::Severity::Error, "Could not load translations.");

    // setup basic configuration
    IConfiguration* configuration = m_coreFactory->getConfiguration();
    configuration->setDefaultValue(ExternalToolsConfigKeys::aisPath, QStandardPaths::findExecutable("align_image_stack"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::convertPath, QStandardPaths::findExecutable("convert"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::ffmpegPath, QStandardPaths::findExecutable("ffmpeg"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::ffprobePath, QStandardPaths::findExecutable("ffprobe"));

    const QVariant ffmpegPath = configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
    const QFileInfo fileInfo(ffmpegPath.toString());

    if (fileInfo.isExecutable() == false)
        gui_logger->warning("Path to FFMpeg tool is invalid. Thumbnails for video files will not be available.");

    //
    auto thumbnail_generator_logger = loggerFactory->get("ThumbnailGenerator");
    ThumbnailGenerator thbGen(m_coreFactory->getTaskExecutor(), thumbnail_generator_logger.get(), configuration);
    ThumbnailManager thbMgr(&thbGen);

    // main window
    MainWindow mainWindow(m_coreFactory, &thbMgr);

    mainWindow.set(m_prjManager);
    mainWindow.set(m_pluginLoader);

    // updater
#ifdef UPDATER_ENABLED
    Updater updater;
    mainWindow.set(&updater);
#endif

    // features
    ImagesDetector img_det(gui_logger->subLogger("ImagesDetector"));
    auto* detector = m_coreFactory->getFeaturesManager();
    detector->add(&img_det);
    detector->detect();

    mainWindow.show();
    m_app->exec();
}
