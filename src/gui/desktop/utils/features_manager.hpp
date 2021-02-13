
#ifndef FEATURESMANAGER_H
#define FEATURESMANAGER_H

#include <set>

#include <core/iconfiguration.hpp>
#include <core/ifeatures_manager.hpp>
#include <core/ilogger.hpp>

#include "inotifications.hpp"


class FeaturesManager: public IFeaturesManager
{
public:
    FeaturesManager(INotifications &, IConfiguration &, std::unique_ptr<ILogger> &);

    bool has(const QString &) const override;

private:
    std::set<QString> m_features;
    std::map<QString, int> m_featuresWarnings;
    std::unique_ptr<ILogger> m_logger;
    IConfiguration& m_configuration;
    INotifications& m_notifications;

    void refresh();

    void refreshTools();
    void refreshImageFormats();

    void addFeature(const QString &);
    void removeFeature(const QString &);

    void configChanged(const QString &, const QVariant &);
    void testTool(const QString &, const QString &, const QString &);
};

#endif
