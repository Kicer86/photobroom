
#include <future>

#include <core/function_wrappers.hpp>
#include "thumbnail_image_provider.hpp"


namespace
{
    class AsyncImageResponse : public QQuickImageResponse
    {
        public:
            AsyncImageResponse(const Photo::Id &id, const QSize &requestedSize, IThumbnailsManager& thbMgr)
            {
                thbMgr.fetch(id, requestedSize, [this](const QImage& thumb)
                {
                    invokeMethod(this, &AsyncImageResponse::handleDone, thumb);
                });
            }

            void handleDone(QImage image)
            {
                m_image = image;
                emit finished();
            }

            QQuickTextureFactory *textureFactory() const override
            {
                return QQuickTextureFactory::textureFactoryForImage(m_image);
            }

            QImage m_image;
    };
}


ThumbnailImageProvider::ThumbnailImageProvider(IThumbnailsManager& mgr)
    : QQuickAsyncImageProvider()
    , m_thumbMgr(mgr)
{

}


QQuickImageResponse* ThumbnailImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    bool status = true;
    const int rawId = id.toInt(&status);
    assert(status && rawId != -1);

    const Photo::Id ph_id(rawId);

    return new AsyncImageResponse(ph_id, requestedSize, m_thumbMgr);
}
