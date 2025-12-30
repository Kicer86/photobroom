
#include <QFileInfo>
#include <QPromise>

#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/itask_executor.hpp>
#include <core/media_types.hpp>
#include <core/task_executor_utils.hpp>
#include <database/database_executor_traits.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"

namespace
{
    const QString MultimediaType("multimedia_type");

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
}


MediaViewCtrl::MediaViewCtrl()
    : m_initializer(this, {"core", "photoID"}, std::bind(&MediaViewCtrl::process, this))
{

}


MediaViewCtrl::~MediaViewCtrl()
{
    m_pathFetchFuture.cancel();
    m_pathFetchFuture.waitForFinished();
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


void MediaViewCtrl::process()
{
    Database::IDatabase* db = ObjectsAccessor::instance().database();
    assert(db);

    QPromise<std::pair<QUrl, Mode>> promise;
    m_pathFetchFuture = promise.future();
    m_pathFetchFuture.then(this, [this](const std::pair<QUrl, Mode>& url_and_mode)
    {
        setPath(url_and_mode.first);
        setMode(url_and_mode.second);
    });

    auto task = [core = m_core, db, id = m_id, promise = std::move(promise)](Database::IBackend& backend) mutable
    {
        const auto data = backend.getPhotoDelta<Photo::Field::Path>(id);
        const auto path = data.get<Photo::Field::Path>();
        const QFileInfo pathInfo(path);
        const auto url = QUrl::fromLocalFile(pathInfo.absoluteFilePath());       // QML's MediaPlayer does not like 'prj:' prefix
        const auto state = backend.get(id, MultimediaType);

        if (state)
        {
            const Mode mode = static_cast<Mode>(*state);

            promise.addResult(std::make_pair(url, mode));
            promise.finish();
        }
        else
        {
            runOn(core->getTaskExecutor(), [core, db, id, url, promise = std::move(promise)]() mutable
            {
                const Mode mode = getFileType(*core, url);
                db->exec([id, mode](Database::IBackend& backend)
                {
                    backend.set(id, MultimediaType, mode);
                });

                promise.addResult(std::make_pair(url, mode));
                promise.finish();
            });
        }
    };

    db->exec(std::move(task));
}
