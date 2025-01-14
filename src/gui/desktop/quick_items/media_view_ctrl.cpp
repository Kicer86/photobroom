
#include <QFileInfo>
#include <QPromise>

#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/media_types.hpp>
#include <core/task_executor_utils.hpp>
#include <database/database_executor_traits.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"


namespace
{
    MediaViewCtrl::Mode getModeFor(ICoreFactoryAccessor* core, const QString& path)
    {
        MediaViewCtrl::Mode mode = MediaViewCtrl::Mode::Unknown;

        if (MediaTypes::isAnimatedImageFile(path))
            mode = MediaViewCtrl::Mode::AnimatedImage;
        else if (MediaTypes::isImageFile(path))
        {
            auto& exifReader = core->getExifReaderFactory().get();
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


MediaViewCtrl::~MediaViewCtrl()
{
    m_pathFetchFuture.cancel();
}


void MediaViewCtrl::setSource(const Photo::Id& id)
{
    if (m_core == nullptr)
    {
        invokeMethodLater(this, &MediaViewCtrl::setSource, id);
        return;
    }

    assert(id.valid());
    m_id = id;

    emit sourceChanged(id);
    emit photoIDStringChanged(photoIDString());

    Database::IDatabase* db = ObjectsAccessor::instance().database();

    if (db && id.valid())
    {
        QPromise<std::pair<QUrl, Mode>> promise;
        m_pathFetchFuture = promise.future();
        m_pathFetchFuture.then(this, [this](const std::pair<QUrl, Mode>& url_and_mode)
        {
            setPath(url_and_mode.first);
            setMode(url_and_mode.second);
        });

        runOn(m_core->getTaskExecutor(), [db, id, core = m_core, promise = std::move(promise)]() mutable
        {
            const auto data = evaluate(*db, [id](Database::IBackend& backend)
            {
                return backend.getPhotoDelta<Photo::Field::Path>(id);
            });

            const auto path = data.get<Photo::Field::Path>();
            const QFileInfo pathInfo(path);
            const auto url = QUrl::fromLocalFile(pathInfo.absoluteFilePath());       // QML's MediaPlayer does not like 'prj:' prefix
            const auto mode = getModeFor(core, path);

            promise.addResult(std::make_pair(url, mode));
            promise.finish();
        });
    }
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
