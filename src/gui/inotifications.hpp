
#ifndef INOTIFICATIONS_HPP_INCLUDED
#define INOTIFICATIONS_HPP_INCLUDED

#include <QString>

#include <core/id.hpp>


class INotifications
{
public:
    struct NotificationsTag;
    using Id = ::Id<int, NotificationsTag>;

    enum class Type
    {
        Warning,
        Info,
    };

    virtual Id insert(const QString &, Type) = 0;
    virtual void remove(Id id) = 0;
};

#endif
