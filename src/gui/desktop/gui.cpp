
#include "gui.hpp"

#include <QApplication>
#include <QFileInfo>
#include <QImageReader>
#include <QStandardPaths>
#include <QTranslator>

#ifdef WIN32
    #include <QQuickStyle>
#endif

#include <core/constants.hpp>
#include <core/iconfiguration.hpp>
#include <core/icore_factory_accessor.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger.hpp>
#include <core/itask_executor.hpp>
#include <core/ilogger_factory.hpp>
#include <core/thumbnail_generator.hpp>
#include <core/thumbnail_manager.hpp>
#include <core/thumbnails_cache.hpp>
#include <system/filesystem.hpp>

#ifdef UPDATER_ENABLED
#include <updater/updater.hpp>
#endif

#include "ui/mainwindow.hpp"
#include "quick_views/qml_setup.hpp"
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

            for(const QByteArray image: qAsConst(images))
            {
                const QString msg = QString("Qt supports %1 file format").arg(image.data());

                m_logger->debug(msg);

                if (image == "mng")
                    features.append(gui::features::MngFile);
            }

            return features;
        }

        std::unique_ptr<ILogger> m_logger;
    };

    struct ThumbnailUtils: IThumbnailUtils
    {
        ThumbnailUtils(ILogger* logger, IConfiguration* config): m_gen(logger, config)
        {

        }

        IThumbnailsCache* cache() override
        {
            return &m_cache;
        }

        IThumbnailsGenerator* generator() override
        {
            return &m_gen;
        }

        ThumbnailsCache m_cache;
        ThumbnailGenerator m_gen;
    };
}


Gui::Gui(IProjectManager& prjMgr, IPluginLoader& pluginLoader, ICoreFactoryAccessor& coreFactory):
    m_prjManager(prjMgr),
    m_pluginLoader(pluginLoader),
    m_coreFactory(coreFactory)
{
    register_qml_types();

    qRegisterMetaType<QVector<QRect>>("QVector<QRect>");
    qRegisterMetaType<TagTypes>("TagTypes");
    qRegisterMetaType<std::vector<Photo::Id>>("std::vector<Photo::Id>");
    qRegisterMetaType<std::set<Photo::Id>>("std::set<Photo::Id>");
    qRegisterMetaType<Photo::Id>("Photo::Id");
    qRegisterMetaType<IPhotoInfo::Ptr>("IPhotoInfo::Ptr");
}


Gui::~Gui()
{

}


void Gui::run()
{
#ifdef GUI_STATIC
    // see: http://doc.qt.io/qt-5/resources.html
    Q_INIT_RESOURCE(images);
#endif

#ifdef OS_WIN
    // On Windows, add extra location for Qt plugins
    qApp->addLibraryPath(FileSystem().getLibrariesPath());

    // Default style is ugly, switch to something nice
    QQuickStyle::setStyle("Fusion");
#endif

    ILoggerFactory* loggerFactory = m_coreFactory.getLoggerFactory();

    auto gui_logger = loggerFactory->get("Gui");
    auto photos_manager_logger = loggerFactory->get("Photos manager");

    const QString tr_path = FileSystem().getTranslationsPath();
    gui_logger->info(QString("Searching for translations in: %1").arg(tr_path));

    // translations
    const QLocale locale;

    const QString info = QString("System language: %1").arg(locale.name()).replace('_', "-");
    gui_logger->debug(info);

    const auto uiLangs = locale.uiLanguages();
    const QString uiLangsStr = uiLangs.join(", ");
    const QString language_details = QString("List of UI langauges: %1").arg(uiLangsStr);
    gui_logger->debug(language_details);

    bool translations_status = false;
    QTranslator translator;
    if (translator.load(locale, QLatin1String("photo_broom"), QLatin1String("_"), tr_path))
        translations_status = QCoreApplication::installTranslator(&translator);

    if (translations_status)
    {
        gui_logger->log(ILogger::Severity::Info, "Translations loaded successfully.");
        gui_logger->log(ILogger::Severity::Debug, QString("Loaded %1 translation from file: %2")
            .arg(translator.language())
            .arg(translator.filePath())
        );
    }
    else
        gui_logger->log(ILogger::Severity::Error, "Could not load translations.");

    // setup basic configuration
    IConfiguration* configuration = m_coreFactory.getConfiguration();

    // defaults
    configuration->setDefaultValue(ExternalToolsConfigKeys::aisPath, QStandardPaths::findExecutable("align_image_stack"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::magickPath, QStandardPaths::findExecutable("magick"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::ffmpegPath, QStandardPaths::findExecutable("ffmpeg"));
    configuration->setDefaultValue(ExternalToolsConfigKeys::ffprobePath, QStandardPaths::findExecutable("ffprobe"));

    const QVariant ffmpegPath = configuration->getEntry(ExternalToolsConfigKeys::ffmpegPath);
    const QFileInfo fileInfo(ffmpegPath.toString());

    if (fileInfo.isExecutable() == false)
        gui_logger->warning("Path to FFMpeg tool is invalid. Thumbnails for video files will not be available.");

    //
    auto thumbnail_generator_logger = loggerFactory->get("ThumbnailGenerator");
    ThumbnailUtils thbUtils(thumbnail_generator_logger.get(), configuration);
    ThumbnailManager thbMgr(m_coreFactory.getTaskExecutor(), thbUtils.generator(), thbUtils.cache());

    // main window
    MainWindow mainWindow(&m_coreFactory, &thbMgr);

    mainWindow.set(&m_prjManager);
    mainWindow.set(&m_pluginLoader);

    // updater
#ifdef UPDATER_ENABLED
    Updater updater;
    mainWindow.set(&updater);
#endif

    // features
    ImagesDetector img_det(gui_logger->subLogger("ImagesDetector"));
    auto* detector = m_coreFactory.getFeaturesManager();
    detector->add(&img_det);
    detector->detect();

    mainWindow.show();

    qApp->exec();
}
