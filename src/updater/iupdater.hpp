
#ifndef IUPDATER_HPP
#define IUPDATER_HPP

#include <QUrl>

struct IUpdater
{
    struct OnlineVersion
    {
        QUrl url;
        enum
        {
            ConnectionError,
            NewVersionAvailable,
            UpToDate
        } status;

        OnlineVersion(): url(), status(ConnectionError) {}
    };

    typedef std::function<void(const OnlineVersion &)> StatusCallback;

    virtual ~IUpdater() {}

    virtual void getStatus(const StatusCallback &) = 0;
};

#endif // IUPDATER_HPP
