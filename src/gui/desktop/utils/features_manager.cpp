
#include <QFileInfo>
#include <QImageReader>
#include <QVariant>

#include <core/constants.hpp>

#include "features_manager.hpp"
#include "gui/features.hpp"

using namespace std::placeholders;


FeaturesManager::FeaturesManager(IConfiguration& configuration, const std::unique_ptr<ILogger>& logger)
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
