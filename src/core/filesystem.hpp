
#ifndef FILESYSTEM_HPP_INCLUDED
#define FILESYSTEM_HPP_INCLUDED

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#include <QByteArrayView>
#include <QString>

#include "core_export.h"


namespace Filesystem
{
    class CORE_EXPORT IFile
    {
    public:
        virtual ~IFile() = default;

        virtual QByteArrayView asQArrayView() const = 0;
        virtual std::span<const std::uint8_t> byteView() const = 0;

        [[deprecated("Use byteView() or asQArrayView() instead")]]
        virtual std::string path() const = 0;
    };

    class CORE_EXPORT Location
    {
    public:
        Location() = default;
        explicit Location(const QStringView &);
        explicit Location(const std::string_view &);
        auto operator<=>(const Location &) const = default;

        QString toQStr() const;
        std::string toStr() const;

    private:
        QString m_location;
    };

    CORE_EXPORT std::unique_ptr<IFile> openFile(const Location &);
}

#endif
