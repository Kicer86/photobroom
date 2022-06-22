
#ifndef INOTIFICATIONS_HPP_INCLUDED
#define INOTIFICATIONS_HPP_INCLUDED

#include <QString>

class INotifications
{
public:
    enum class Type
    {
        Warning,
        Info,
    };

    virtual int insert(const QString &, Type) = 0;
    virtual void removeId(int id) = 0;
};

#endif
