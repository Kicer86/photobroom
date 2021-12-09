
#ifndef THUMBNAILIMAGEPROVIDER_HPP
#define THUMBNAILIMAGEPROVIDER_HPP

#include <QQuickAsyncImageProvider>

#include "utils/ithumbnails_manager.hpp"


class ThumbnailImageProvider: public QQuickAsyncImageProvider
{
public:
    explicit ThumbnailImageProvider(IThumbnailsManager &);

    QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
    IThumbnailsManager& m_thumbMgr;
};

#endif
