
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <optional>
#include <QSize>
#include <QDateTime>


struct MediaFile
{
    std::optional<QSize> dimension;
    std::optional<QDateTime> creationTime;
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

    virtual FileInformation getInformation(const QString &) const = 0;
};

#endif
