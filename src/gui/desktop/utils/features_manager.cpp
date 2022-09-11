
#include <QFileInfo>
#include <QImageReader>
#include <QVariant>

#include <core/constants.hpp>

#include "features_manager.hpp"
#include "gui/features.hpp"

using namespace std::placeholders;

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
        { "AlignImageStack", gui::features::ToolAIS      ,ExternalToolsConfigKeys::aisPath     },
        { "ExifTool",        gui::features::ToolExifTool ,ExternalToolsConfigKeys::exiftoolPath }
    };
}


FeaturesManager::FeaturesManager(IConfiguration& configuration, std::unique_ptr<ILogger>& logger)
    : m_logger(logger->subLogger("FeaturesManager"))
    , m_configuration(configuration)
{
    refresh();

    for (const auto& tool: Tools)
    {
        const QString& configKey = tool.configurationKey;
        m_configuration.watchFor(configKey, std::bind(&FeaturesManager::configChanged, this, _1, _2));
    }
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
        const QString& configKey = tool.configurationKey;
        const QString path = m_configuration.getEntry(configKey).toString();

        testTool(path, tool.featureKey, tool.name);
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
    const auto info = m_features.insert(feature);

    if (info.second)
    {
        m_logger->debug(QString("Enabling %1 feature").arg(feature));

        emit featureChanged(feature, true);
    }
}


void FeaturesManager::removeFeature(const QString& feature)
{
    const auto count = m_features.erase(feature);

    if (count > 0)
    {
        m_logger->debug(QString("Disabling %1 feature").arg(feature));

        emit featureChanged(feature, false);
    }
}


void FeaturesManager::configChanged(const QString& changed_key, const QVariant& new_value)
{
    for (const auto& tool: Tools)
        if (tool.configurationKey == changed_key)
        {
            const QString path = new_value.toString();

            testTool(path, tool.featureKey, tool.name);
        }
}

void FeaturesManager::testTool(const QString& path, const QString& featureKey, const QString& toolName)
{
    const QFileInfo toolInfo(path);

    if (toolInfo.exists() && toolInfo.isExecutable())
        addFeature(featureKey);
    else
    {
        const QString& name = toolName;

        m_logger->warning(QString("Path '%1' for tool %2 does not exist or file is not executable.").arg(path).arg(name));

        removeFeature(featureKey);
    }
}
