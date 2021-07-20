
#ifndef ITHUMBNAILS_MANAGER_HPP
#define ITHUMBNAILS_MANAGER_HPP

#include <functional>
#include <optional>
#include <QSize>

#include <database/idatabase.hpp>


struct IThumbnailsManager
{
    virtual ~IThumbnailsManager() = default;

    // Request thumbnail. Third parameter is a callback which will be called as soon as thumbnail is accessible.
    virtual void fetch(const Photo::Id& id, const QSize& desired_size, const std::function<void(const QImage &)> &) = 0;

    // Return thumbnail if immediately accessible. Otherwise result is empty.
    virtual std::optional<QImage> fetch(const Photo::Id& id, const QSize& desired_size) = 0;

    virtual void setDatabaseCache(Database::IDatabase *) = 0;
};

#endif
