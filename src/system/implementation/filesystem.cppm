module;

class QString;
#include "system_export.h"

export module broom.system:filesystem;

export struct SYSTEM_EXPORT FileSystem
{
    QString getPluginsPath() const;
    QString getTranslationsPath() const;
    QString getLibrariesPath() const;
    QString getScriptsPath() const;
    QString getDataPath() const;

    QString commonPath(const QString&, const QString&) const;

private:
    QString read(const char*) const;
};
