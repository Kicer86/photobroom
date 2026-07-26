
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

    CORE_EXPORT std::unique_ptr<IFile> openFile(std::string_view path);
    CORE_EXPORT std::unique_ptr<IFile> openFile(const QString& path);
}

#endif // FILESYSTEM_HPP_INCLUDED
