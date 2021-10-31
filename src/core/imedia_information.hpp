
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <optional>

#include <QSize>


struct MediaFile
{
    QSize dimension;
};

struct ImageFile
{
};

struct VideoFile
{
};

struct FileInformation
{
    MediaFile common;
    std::variant<ImageFile, VideoFile> details;
};


struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual std::optional<QSize> size(const QString &) const = 0;
    virtual FileInformation getInformation(const QString &) const = 0;
};

#endif
