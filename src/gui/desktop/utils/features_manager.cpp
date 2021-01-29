
#include <QFileInfo>
#include <QImageReader>
#include <QVariant>

#include <core/constants.hpp>

#include "features_manager.hpp"
#include "features.hpp"

namespace
{
     struct ToolInfo
    {
        QString name;
        QString featureKey;
        QString configurationKey;
    };

    std::vector<ToolInfo> Tools =
    {
        { "Magick",          gui::features::ToolMagick  ,ExternalToolsConfigKeys::magickPath  },
        { "AlignImageStack", gui::features::ToolAIS     ,ExternalToolsConfigKeys::aisPath     },
        { "FFMpeg",          gui::features::ToolFFMpeg  ,ExternalToolsConfigKeys::ffmpegPath  },
        { "FFProbe",         gui::features::ToolFFProbe ,ExternalToolsConfigKeys::ffprobePath }
    };
}


FeaturesManager::FeaturesManager(IConfiguration& configuration, std::unique_ptr<ILogger>& logger)
    : m_logger(logger->subLogger("FeaturesManager"))
    , m_configuration(configuration)
{
    refresh();
}


bool FeaturesManager::has(const QString& name) const
{
    return m_features.contains(name);
}


void FeaturesManager::refresh()
{
    refreshTools();
    refreshImageFormats();
}


void FeaturesManager::refreshTools()
{
    for (const auto& tool: Tools)
    {
        const QString& key = tool.featureKey;
        const QString& configKey = tool.configurationKey;
        const QString path = m_configuration.getEntry(configKey).toString();
        const QFileInfo toolInfo(path);

        if (toolInfo.exists() && toolInfo.isExecutable())
            addFeature(key);
        else
        {
            const QString& name = tool.name;

            m_logger->warning(QString("Path '%1' for tool %2 does not exist or file is not executable.").arg(path).arg(name));

            removeFeature(key);
        }
    }
}


void FeaturesManager::refreshImageFormats()
{
    QList<QByteArray> images = QImageReader::supportedImageFormats();

    bool mngDetected = false;

    for(const QByteArray& image: qAsConst(images))
    {
        const QString msg = QString("Qt supports %1 file format").arg(image.data());

        m_logger->debug(msg);

        if (image == "mng")
            mngDetected = true;
    }

    mngDetected? addFeature(gui::features::MngFile) : removeFeature(gui::features::MngFile);
}


void FeaturesManager::addFeature(const QString& feature)
{
    m_features.insert(feature);

    m_logger->debug(QString("Enabling %1 feature").arg(feature));
}


void FeaturesManager::removeFeature(const QString& feature)
{
    m_features.erase(feature);

    m_logger->debug(QString("Disabling %1 feature").arg(feature));
}
