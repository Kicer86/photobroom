
#include "media_types.hpp"

#include <QFileInfo>
#include <QRegExp>
#include <QString>

namespace
{
    const QRegExp img_regex("jpe?g|png|tif?f|bmp", Qt::CaseInsensitive);
    const QRegExp vid_regex("gif|avi|mkv|mpe?g|mp4", Qt::CaseInsensitive);
}


namespace MediaTypes
{

    bool isImageFile(const QString& file_path)
    {
        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = img_regex.exactMatch(ext);

        return matches;
    }

    bool isVideoFile(const QString& file_path)
    {
        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = vid_regex.exactMatch(ext);

        return matches;
    }
}
