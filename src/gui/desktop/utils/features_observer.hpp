
#ifndef FEATURESOBSERVER_HPP
#define FEATURESOBSERVER_HPP

#include <QObject>

#include "models/notifications_model.hpp"
#include "core/ifeatures_manager.hpp"


class FeaturesObserver: public QObject
{
    Q_OBJECT

public:
    FeaturesObserver(IFeaturesManager &, NotificationsModel &);

private:
    std::map<QString, int> m_warnings;
    NotificationsModel& m_model;

    void featureChanged(const QString& key, bool value);
};

#endif // FEATURESOBSERVER_HPP
