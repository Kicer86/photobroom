
#include <core/task_executor_utils.hpp>

#include "../db_thumbnails_accessor.hpp"
#include "database_executor_traits.hpp"

namespace Database
{
    ThumbnailsAccessor::ThumbnailsAccessor(IDatabase& db)
        : m_db(db)
    {

    }

    QImage ThumbnailsAccessor::getThumbnail(const Photo::Id& id)
    {
        const QByteArray rawImage = evaluate<QByteArray(Database::IBackend &)>(m_db, [](IBackend& backend)
        {
            return QByteArray();
        });

        return QImage();
    }
}
