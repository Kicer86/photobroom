
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
        const bool isImage = mimeName.left(6) == "image/";

        return isImage;
    }

    bool isAnimatedImageFile(const QString& file_path)
    {
        const QMimeType mime = QMimeDatabase().mimeTypeForFile(file_path);
        const QString mimeName = mime.name();
        const bool isAnimated = mimeName == "image/gif";

        return isAnimated;
    }

    bool isVideoFile(const QString& file_path)
    {
        const QMimeType mime = QMimeDatabase().mimeTypeForFile(file_path);
        const QString mimeName = mime.name();
        const bool isVideo = mimeName.left(6) == "video/";

        return isVideo;
    }
}
