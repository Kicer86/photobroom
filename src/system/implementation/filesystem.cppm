module;

class QString;

export module system:filesystem;

export struct FileSystem
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
