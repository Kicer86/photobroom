
#include <QFileInfo>

#include <core/function_wrappers.hpp>
#include <core/media_types.hpp>

#include "media_view_ctrl.hpp"
#include "objects_accessor.hpp"


void MediaViewCtrl::setSource(const Photo::Id& id)
{
    m_id = id;

    emit sourceChanged(id);

    Database::IDatabase* db = ObjectsAccessor::instance().database();

    if (db)
    {
        db->exec([this, id](Database::IBackend& backend)
        {
            const Photo::Data data = backend.getPhoto(id);

            invokeMethod(this, &MediaViewCtrl::setPath, data.path);
        });
    }
}


const Photo::Id& MediaViewCtrl::source() const
{
    return m_id;
}


QString MediaViewCtrl::path() const
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

    m_path = pathInfo.absoluteFilePath();       // QML's MediaPlayer does not like 'prj:' prefix
    emit pathChanged(m_path);

    if (MediaTypes::isImageFile(m_path))
        setMode(Mode::StaticImage);
    else if (MediaTypes::isVideoFile(m_path))
        setMode(Mode::Video);
}


void MediaViewCtrl::setMode(Mode mode)
{
    m_mode = mode;
    emit modeChanged(m_mode);
}
