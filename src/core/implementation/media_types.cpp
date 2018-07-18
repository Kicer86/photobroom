
#include "media_types.hpp"

#include <mutex>

#include <QFileInfo>
#include <QRegExp>
#include <QString>

namespace
{
    const QRegExp img_regex("jpe?g|png|gif|tif?f|bmp", Qt::CaseInsensitive);
    const QRegExp anim_img_regex("gif", Qt::CaseInsensitive);
    const QRegExp vid_regex("avi|mkv|mpe?g|mp4", Qt::CaseInsensitive);

    std::mutex img_regex_mutex;
    std::mutex anim_img_regex_mutex;
    std::mutex vid_regex_mutex;
}


namespace MediaTypes
{

    bool isImageFile(const QString& file_path)
    {
        std::lock_guard<std::mutex> lock(img_regex_mutex);

        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = img_regex.exactMatch(ext);

        return matches;
    }

    bool isAnimatedImageFile(const QString& file_path)
    {
        std::lock_guard<std::mutex> lock(anim_img_regex_mutex);

        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = anim_img_regex.exactMatch(ext);

        return matches;
    }

    bool isVideoFile(const QString& file_path)
    {
        std::lock_guard<std::mutex> lock(vid_regex_mutex);

        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = vid_regex.exactMatch(ext);

        return matches;
    }
}
