
#include <optional>
#include <utility>

#include <QFileInfo>
#include <QCoroTask>

#include <core/iexif_reader.hpp>
#include <core/media_types.hpp>
#include <core/task_executor_qcoro_utils.hpp>
#include <database/database_qcoro_utils.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"

namespace
{
    const QString MultimediaType("multimedia_type");
    using MediaInfo = std::pair<QUrl, MediaViewCtrl::Mode>;

    struct DbMediaData
    {
        QUrl url;
        std::optional<MediaViewCtrl::Mode> mode;
    };

    MediaViewCtrl::Mode getFileType(ICoreFactoryAccessor& core, const QUrl& url)
    {
        const auto path = url.toLocalFile();
        MediaViewCtrl::Mode mode = MediaViewCtrl::Mode::Unknown;

        if (MediaTypes::isAnimatedImageFile(path))
            mode = MediaViewCtrl::Mode::AnimatedImage;
        else if (MediaTypes::isImageFile(path))
        {
            auto& exifReader = core.getExifReaderFactory().get();
            const auto projection = exifReader.get(path, IExifReader::TagType::Projection);
            if (projection && std::any_cast<std::string>(*projection) == "equirectangular")
                mode = MediaViewCtrl::Mode::EquirectangularProjectionImage;
            else
                mode = MediaViewCtrl::Mode::StaticImage;
        }
        else if (MediaTypes::isVideoFile(path))
            mode = MediaViewCtrl::Mode::Video;
        else
            mode = MediaViewCtrl::Mode::Error;

        return mode;
    }

    QCoro::Task<DbMediaData> readMediaData(Database::IDatabase& database, const Photo::Id& id)
    {
        co_return co_await Database::coRunOn(
            database,
            [id](Database::IBackend& backend)
            {
                const auto data = backend.getPhotoDelta<Photo::Field::Path>(id);
                const auto path = data.get<Photo::Field::Path>();
                const QFileInfo pathInfo(path);
                const auto url = QUrl::fromLocalFile(pathInfo.absoluteFilePath());       // QML's MediaPlayer does not like 'prj:' prefix
                const auto state = backend.get(id, MultimediaType);

                if (state)
                {
                    const MediaViewCtrl::Mode mode = static_cast<MediaViewCtrl::Mode>(*state);
                    return DbMediaData{url, mode};
                }

                return DbMediaData{url, std::nullopt};
            },
            "MediaViewCtrl: update mode"
        );
    }

    QCoro::Task<MediaInfo> getMediaInfo(ICoreFactoryAccessor& core, Database::IDatabase& database, const Photo::Id& id)
    {
        const DbMediaData data = co_await readMediaData(database, id);

        if (data.mode)
            co_return std::make_pair(data.url, *data.mode);
        else
        {
            const auto mode = co_await coRunOn(
                core.getTaskExecutor(),
                [core = &core, url = data.url]()
                {
                    return getFileType(*core, url);
                },
                "MediaViewCtrl: detect file type"
            );

            database.exec([id, mode](Database::IBackend& storage_backend)
            {
                storage_backend.set(id, MultimediaType, mode);
            }, "MediaViewCtrl: set file type");

            co_return std::make_pair(data.url, mode);
        }
    }
}


MediaViewCtrl::MediaViewCtrl()
    : m_initializer(this, {"core", "photoID"}, std::bind(&MediaViewCtrl::process, this))
{

}


void MediaViewCtrl::setSource(const Photo::Id& id)
{
    assert(id.valid());
    m_id = id;

    emit sourceChanged(id);
    emit photoIDStringChanged(photoIDString());
}


const Photo::Id& MediaViewCtrl::source() const
{
    return m_id;
}


QString MediaViewCtrl::photoIDString() const
{
    return QString::number(m_id.value());
}


QUrl MediaViewCtrl::path() const
{
    return m_path;
}


MediaViewCtrl::Mode MediaViewCtrl::mode() const
{
    return m_mode;
}


void MediaViewCtrl::setPath(const QUrl& path)
{
    m_path = path;
    emit pathChanged(m_path);
}


void MediaViewCtrl::setMode(Mode mode)
{
    m_mode = mode;
    emit modeChanged(m_mode);
}


void MediaViewCtrl::applyMediaInfo(const std::pair<QUrl, Mode>& mediaInfo)
{
    setPath(mediaInfo.first);
    setMode(mediaInfo.second);
}


void MediaViewCtrl::process()
{
    Database::IDatabase* db = ObjectsAccessor::instance().database();
    assert(db);
    assert(m_core);

    QCoro::connect(getMediaInfo(*m_core, *db, m_id), this, &MediaViewCtrl::applyMediaInfo);
}
