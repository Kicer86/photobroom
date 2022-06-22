
#include "features_observer.hpp"

#include "gui/features.hpp"


namespace
{
    const std::map<QString, QString> Tools =
    {
        { gui::features::ToolMagick   ,"Magick"          },
        { gui::features::ToolAIS      ,"AlignImageStack" },
        { gui::features::ToolFFMpeg   ,"FFMpeg"          },
        { gui::features::ToolExifTool ,"ExifTool"        }
    };
}


FeaturesObserver::FeaturesObserver(IFeaturesManager& features, INotifications& notifications)
    : m_notifications(notifications)
{
    connect(&features, &IFeaturesManager::featureChanged, this, &FeaturesObserver::featureChanged);

    for(const auto& tool: Tools)
        featureChanged(tool.first, features.has(tool.first));
}


void FeaturesObserver::featureChanged(const QString& feature, bool value)
{
    if (Tools.contains(feature))
    {
        if (value)
        {
            auto it = m_warnings.find(feature);

            if (it != m_warnings.end())
            {
                m_notifications.remove(it->second);
                m_warnings.erase(it);
            }
        }
        else
        {
            auto it = m_warnings.find(feature);

            if (it == m_warnings.end())
            {
                auto toolIt = Tools.find(feature);

                const auto id = m_notifications.insert(tr("Path for tool %1 is not set or is invalid.\n"
                                                          "Some functionality may be disabled.\n"
                                                          "Check paths in configuration window.").arg(toolIt->second),
                                                       INotifications::Type::Warning);

                m_warnings.emplace(feature, id);
            }
        }
    }
}
