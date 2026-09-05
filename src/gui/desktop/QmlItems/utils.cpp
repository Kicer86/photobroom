
#include <QCoreApplication>
#include <QQmlEngine>

#include <desktop_services.hpp>
#include "utils.hpp"


namespace
{
    void registerSingleton()
    {
        qmlRegisterSingletonType<Utils>("QmlItems", 1, 0, "Utils", [](QQmlEngine *, QJSEngine *)
        {
            return new Utils;
        });

    }
}

Q_COREAPP_STARTUP_FUNCTION(registerSingleton)


bool Utils::openContainingDirectory(const Filesystem::Location& photo)
{
    return DesktopServices::openContainingDirectory(photo);
}
