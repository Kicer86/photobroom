#ifndef INOTIFICATIONS_HPP_INCLUDED
#define INOTIFICATIONS_HPP_INCLUDED

#include <QString>

class INotifications
{
public:
    virtual ~INotifications() = default;

    virtual void reportWarning(const QString &) = 0;
};

#endif // INOTIFICATIONS_HPP_INCLUDED
