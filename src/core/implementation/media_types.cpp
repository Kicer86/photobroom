
#include <mutex>

#include <QMimeDatabase>
#include <QSet>

#include "containers_utils.hpp"
#include "media_types.hpp"


namespace MediaTypes
{
    namespace
    {
        /**
         * This function does what QMimeDatabase::mimeTypeForFile is doing, but splits it into steps so QMimeDatabase does not open file on its own.
         * It is required as Filesystem::Location is being used and it may point to some virtual files.
         */
        QString fetchMimeTypeName(const Filesystem::Location& location)
        {
            QMimeDatabase mimeDB;

            // try to guess mime type from file name
            const auto mimeTypes = mimeDB.mimeTypesForFileName(location.url());
            const auto mimeNames = mimeTypes | std::views::transform([](const auto& m){ return m.name(); }) | std::ranges::to<QSet<QString>>();

            if (mimeNames.size() == 1)
                return front(mimeNames);
            else
            {
                auto file = Filesystem::openFile(location);
                auto mimeType = mimeDB.mimeTypeForData(&*file);

                return mimeType.name();
            }
        }
    }

    bool isImageFile(const Filesystem::Location& location)
    {
        const QString mimeName = fetchMimeTypeName(location);
        const bool isImage = mimeName.left(6) == "image/" || mimeName == "video/x-mng";

        return isImage;
    }

    bool isAnimatedImageFile(const Filesystem::Location& location)
    {
        const QString mimeName = fetchMimeTypeName(location);
        const bool isAnimated = mimeName == "image/gif" || mimeName == "video/x-mng" || mimeName == "image/webp";

        return isAnimated;
    }

    bool isVideoFile(const Filesystem::Location& location)
    {
        const QString mimeName = fetchMimeTypeName(location);
        const bool isVideo = mimeName.left(6) == "video/" && mimeName != "video/x-mng";

        return isVideo;
    }
}
