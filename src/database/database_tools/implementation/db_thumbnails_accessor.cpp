
#include <QBuffer>

#include <core/task_executor_utils.hpp>

#include "../db_thumbnails_accessor.hpp"
#include "database_executor_traits.hpp"


namespace Database
{
    ThumbnailsAccessor::ThumbnailsAccessor(IDatabase& db, IThumbnailsGenerator& generator)
        : m_db(db)
        , m_generator(generator)
    {

    }

    QImage ThumbnailsAccessor::getThumbnail(const Photo::Id& id)
    {
        const QByteArray thumbnail =
            evaluate<QByteArray(Database::IBackend &)>(m_db, [id, this](IBackend& backend)
            {
                QByteArray thumbnail = backend.getThumbnail(id);

                if (thumbnail.isEmpty())
                {
                    const auto data = backend.getPhotoDelta(id, {Photo::Field::Path});
                    const auto path = data.get<Photo::Field::Path>();
                    const auto image = m_generator.generate(path, {QSize(400, 400)});

                    QBuffer buf(&thumbnail);
                    image.save(&buf, "JPG");

                    backend.setThumbnail(id, thumbnail);
                }

                return thumbnail;
            });

        QImage image;
        image.load(thumbnail, "JPG");

        return image;
    }
}
