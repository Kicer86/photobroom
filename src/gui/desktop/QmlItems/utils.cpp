
#include <QQmlEngine>

#include "utils.hpp"


namespace
{
    void registerSingleton()
    {
        qmlRegisterSingletonType<Utils>("QmlItems", 1, 0, "Utils", [](QQmlEngine *, QJSEngine *)
        {
            return new Utils;
        });

        qmlRegisterUncreatableType<FileInfo>("QmlItems", 1, 0, "FileInfo", "");
    }
}

Q_COREAPP_STARTUP_FUNCTION(registerSingleton)


FileInfo* Utils::fileInfo()
{
    return new FileInfo;
}
