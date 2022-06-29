
#ifndef FEATURESOBSERVER_HPP
#define FEATURESOBSERVER_HPP

#include <QObject>

#include "inotifications.hpp"
#include "core/ifeatures_manager.hpp"


class FeaturesObserver: public QObject
{
    Q_OBJECT

public:
    FeaturesObserver(IFeaturesManager &, INotifications &);

private:
    std::map<QString, INotifications::Id> m_warnings;
    INotifications& m_notifications;

    void featureChanged(const QString& key, bool value);
};

#endif // FEATURESOBSERVER_HPP
