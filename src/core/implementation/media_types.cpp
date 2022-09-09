
#include "media_types.hpp"

#include <mutex>

#include <QMimeDatabase>
#include <QString>


namespace MediaTypes
{
    bool isImageFile(const QString& file_path)
    {
        const QMimeType mime = QMimeDatabase().mimeTypeForFile(file_path);
        const QString mimeName = mime.name();
        const bool isImage = mimeName.left(6) == "image/" || mimeName == "video/x-mng";

        return isImage;
    }

    bool isAnimatedImageFile(const QString& file_path)
    {
        const QMimeType mime = QMimeDatabase().mimeTypeForFile(file_path);
        const QString mimeName = mime.name();
        const bool isAnimated = mimeName == "image/gif" || mimeName == "video/x-mng" || mimeName == "image/webp";

        return isAnimated;
    }

    bool isVideoFile(const QString& file_path)
    {
        const QMimeType mime = QMimeDatabase().mimeTypeForFile(file_path);
        const QString mimeName = mime.name();
        const bool isVideo = mimeName.left(6) == "video/" && mimeName != "video/x-mng";

        return isVideo;
    }
}
