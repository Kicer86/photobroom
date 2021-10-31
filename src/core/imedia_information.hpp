
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <optional>
#include <QSize>
#include <QDateTime>


struct MediaFile
{
    QSize dimension;
    QDateTime creationTime;
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
