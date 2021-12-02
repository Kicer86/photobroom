
#include <QPainter>

#include <core/iexif_reader.hpp>
#include <core/function_wrappers.hpp>
#include <core/oriented_image.hpp>
#include "media_view_item.hpp"
#include "objects_accessor.hpp"


MediaViewItem::MediaViewItem()
    : m_core(ObjectsAccessor::instance().coreFactory())
{
    assert(m_core != nullptr);

    connect(this, &MediaViewItem::sourceChanged, this, &MediaViewItem::reload);
}


void MediaViewItem::paint(QPainter *painter)
{
    if (m_image.isNull() == false)
    {
        const QSize canvasSize = size().toSize();
        const QImage scaled = m_image.scaled(canvasSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const QRect imgRect(QPoint(), scaled.size());
        QRect paintRect = imgRect;
        paintRect.moveCenter(QPoint(canvasSize.width() / 2, canvasSize.height() / 2));

        painter->drawImage(paintRect.topLeft(), scaled);
    }
}


void MediaViewItem::reload(const Photo::Id& id)
{
    Database::IDatabase* db = ObjectsAccessor::instance().database();

    if (db)
    {
        db->exec([this, id](Database::IBackend& backend)
        {
            const Photo::Data data = backend.getPhoto(id);

            invokeMethod(this, &MediaViewItem::setImage, data.path);
        });
    }
}


void MediaViewItem::setImage(const QString& path)
{
    auto& exifReader = m_core->getExifReaderFactory().get();
    m_image = OrientedImage(exifReader, path).get();

    update();
}
