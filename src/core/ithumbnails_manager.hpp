
#ifndef ITHUMBNAILS_MANAGER_HPP
#define ITHUMBNAILS_MANAGER_HPP

struct IThumbnailsManager
{
    virtual ~IThumbnailsManager() = default;

    virtual void fetch(const QString& path, int desired_height, const std::function<void(int, const QImage &)> &) = 0;
    virtual std::optional<QImage> fetch(const QString& path, int height) = 0;
};

#endif
