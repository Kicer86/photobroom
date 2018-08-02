
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

    bool is(std::mutex& mutex, const QRegExp& regex, const QString& file_path)
    {
        std::lock_guard<std::mutex> lock(mutex);

        const QFileInfo path(file_path);
        const QString ext = path.suffix();
        const bool matches = regex.exactMatch(ext);

        return matches;
    }
}


namespace MediaTypes
{
    bool isImageFile(const QString& file_path)
    {
        return is(img_regex_mutex, img_regex, file_path);
    }

    bool isAnimatedImageFile(const QString& file_path)
    {
        return is(anim_img_regex_mutex, anim_img_regex, file_path);
    }

    bool isVideoFile(const QString& file_path)
    {
        return is(vid_regex_mutex, vid_regex, file_path);
    }
}
