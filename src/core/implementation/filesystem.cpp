
#include "filesystem.hpp"
#include "filesystem_localfile.hpp"


namespace Filesystem
{
    Location::Location(const char* location)
        : Location(QString(location))
    {

    }


    Location::Location(const QStringView& path)
        : Location(QString(path))
    {

    }


    Location::Location(const std::string_view& path)
        : Location(QString::fromUtf8(path.data(), path.size()))
    {

    }


    Location::Location(const QString& location)
        : Location(QStringList(location))
    {

    }


    Location::Location(const QStringList& location)
        : m_location(location)
    {

    }


    QString Location::toQStr() const
    {
        return url();
    }


    std::string Location::toStr() const
    {
        return url().toStdString();
    }


    const QStringList& Location::get() const
    {
        return m_location;
    }


    QString Location::url() const
    {
        return m_location.join("");
    }


    bool Location::isEmpty() const
    {
        return m_location.isEmpty();
    }


    std::unique_ptr<IFile> openFile(const Location& location)
    {
        auto file = std::make_unique<LocalFile>(location.toQStr());
        file->open(QIODeviceBase::ReadOnly);
        return file;
    }

    QString toLocalFile(const Location& location)
    {
        // TODO: improve
        return location.url();
    }
}
