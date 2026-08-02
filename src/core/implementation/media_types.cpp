
#include "media_types.hpp"

#include <mutex>

#include <QMimeDatabase>


namespace MediaTypes
{
    namespace
    {
        /**
         * This function does what QMimeDatabase::mimeTypeForFile is doing, but splits it into steps so QMimeDatabase does not open file on its own.
         * It is required as Filesystem::Location is being used and it may point to some virtual files.
         */
        QMimeType fetchMimeType(const Filesystem::Location& location)
        {
            QMimeDatabase mimeDB;

            // try to guess mime type from file name
            const auto mimeTypes = mimeDB.mimeTypesForFileName(location.url());

            if (mimeTypes.size() == 1)
                return mimeTypes.front();
            else
            {
                auto device = Filesystem::openAsDevice(location);
                auto mimeType = mimeDB.mimeTypeForData(&*device);

                return mimeType;
            }
        }
    }

    bool isImageFile(const Filesystem::Location& location)
    {
        const QMimeType mime = fetchMimeType(location);
        const QString mimeName = mime.name();
        const bool isImage = mimeName.left(6) == "image/" || mimeName == "video/x-mng";

        return isImage;
    }

    bool isAnimatedImageFile(const Filesystem::Location& location)
    {
        const QMimeType mime = fetchMimeType(location);
        const QString mimeName = mime.name();
        const bool isAnimated = mimeName == "image/gif" || mimeName == "video/x-mng" || mimeName == "image/webp";

        return isAnimated;
    }

    bool isVideoFile(const Filesystem::Location& location)
    {
        const QMimeType mime = fetchMimeType(location);
        const QString mimeName = mime.name();
        const bool isVideo = mimeName.left(6) == "video/" && mimeName != "video/x-mng";

        return isVideo;
    }
}
