
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <chrono>
#include <optional>
#include <QSize>
#include <QDateTime>

#include "filesystem.hpp"


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
    std::chrono::milliseconds duration;
};

struct FileInformation
{
    MediaFile common;
    std::variant<ImageFile, VideoFile> details;
};


struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual FileInformation getInformation(const Filesystem::Location &) const = 0;
};

#endif
