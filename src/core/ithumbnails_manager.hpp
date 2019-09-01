
#ifndef ITHUMBNAILS_MANAGER_HPP
#define ITHUMBNAILS_MANAGER_HPP

#include <functional>
#include <optional>

struct IThumbnailsManager
{
    virtual ~IThumbnailsManager() = default;

    // Request thumbnail. Third parameter is a callback which will be called as soon as thumbnail is accessible.
    virtual void fetch(const QString& path, int desired_height, const std::function<void(int, const QImage &)> &) = 0;

    // Return thumbnail if immediately accessible. Otherwise result is empty.
    virtual std::optional<QImage> fetch(const QString& path, int height) = 0;
};

#endif
