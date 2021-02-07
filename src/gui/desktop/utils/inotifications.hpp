
#include <QString>

class INotifications
{
public:
    virtual ~INotifications() = default;

    virtual void reportWarning(const QString &) = 0;
};
