
#include <QFileInfo>
#include <QPromise>

#include <core/function_wrappers.hpp>
#include <core/iexif_reader.hpp>
#include <core/media_types.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"


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

    Database::IDatabase* db = ObjectsAccessor::instance().database();

    if (db && id.valid())
    {
        QPromise<QString> promise;
        m_pathFetchFuture = promise.future();
        m_pathFetchFuture.then(this, std::bind(&MediaViewCtrl::setPath, this, std::placeholders::_1));

        auto task = [id, promise = std::move(promise)](Database::IBackend& backend) mutable
        {
            // MediaViewCtrl may be destroyed here
            const auto data = backend.getPhotoDelta<Photo::Field::Path>(id);

            promise.addResult(data.get<Photo::Field::Path>());
            promise.finish();
        };

        db->exec(std::move(task));
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


void MediaViewCtrl::setPath(const QString& path)
{
    if (m_core == nullptr)
    {
        invokeMethodLater(this, &MediaViewCtrl::setPath, path);
        return;
    }

    setMode(Mode::Unknown);

    const QFileInfo pathInfo(path);

    m_path = QUrl::fromLocalFile(pathInfo.absoluteFilePath());       // QML's MediaPlayer does not like 'prj:' prefix
    emit pathChanged(m_path);

    if (MediaTypes::isAnimatedImageFile(path))
        setMode(Mode::AnimatedImage);
    else if (MediaTypes::isImageFile(path))
    {
        auto& exifReader = m_core->getExifReaderFactory().get();
        const auto projection = exifReader.get(path, IExifReader::TagType::Projection);
        if (projection && std::any_cast<std::string>(*projection) == "equirectangular")
            setMode(Mode::EquirectangularProjectionImage);
        else
            setMode(Mode::StaticImage);
    }
    else if (MediaTypes::isVideoFile(path))
        setMode(Mode::Video);
    else
        setMode(Mode::Error);
}


void MediaViewCtrl::setMode(Mode mode)
{
    m_mode = mode;
    emit modeChanged(m_mode);
}
