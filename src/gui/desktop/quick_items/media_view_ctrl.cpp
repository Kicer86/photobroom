
#include <QFileInfo>

#include <core/function_wrappers.hpp>
#include <core/media_types.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"


MediaViewCtrl::~MediaViewCtrl()
{
    m_callbackCtrl.invalidate();
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
        auto task = m_callbackCtrl.make_safe_callback<Database::IBackend &>([this, id](Database::IBackend& backend)
        {
            // MediaViewCtrl may be destroyed here
            const auto data = backend.getPhotoDelta<Photo::Field::Path>(id);

            invokeMethod(this, &MediaViewCtrl::setPath, data.get<Photo::Field::Path>());
        });

        db->exec(task);
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
    setMode(Mode::Unknown);

    const QFileInfo pathInfo(path);

    m_path = QUrl::fromLocalFile(pathInfo.absoluteFilePath());       // QML's MediaPlayer does not like 'prj:' prefix
    emit pathChanged(m_path);

    if (MediaTypes::isAnimatedImageFile(path))
        setMode(Mode::AnimatedImage);
    else if (MediaTypes::isImageFile(path))
        setMode(Mode::StaticImage);
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
