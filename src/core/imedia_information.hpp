
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <optional>

#include <QSize>


struct MediaFile
{
    QSize dimension;
};

struct ImageFile: MediaFile
{
};

struct VideoFile: MediaFile
{
};

using FileInformation = std::variant<ImageFile, VideoFile>;


struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual std::optional<QSize> size(const QString &) const = 0;
    virtual FileInformation getInformation(const QString &) const = 0;
};

#endif
