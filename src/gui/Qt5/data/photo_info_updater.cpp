
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QPixmap>
#include <QImage>

#include <database/ifs.hpp>
#include <core/task_executor.hpp>
#include <core/hash_functions.hpp>


namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}


struct ThumbnailGenerator: ITaskExecutor::ITask
{
    ThumbnailGenerator(const PhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo) {}
    virtual ~ThumbnailGenerator() {}

    ThumbnailGenerator(const ThumbnailGenerator &) = delete;
    ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

    virtual std::string name() const override
    {
        return "Photo thumbnail generation";
    }

    virtual void perform() override
    {
        QPixmap pixmap(m_photoInfo->getPath().c_str());
        QPixmap thumbnail = pixmap.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->setThumbnail(thumbnail);
    }

    PhotoInfo::Ptr m_photoInfo;
};


struct HashAssigner: public ITaskExecutor::ITask
{
    HashAssigner(const PhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo)
    {
    }

    HashAssigner(const HashAssigner &) = delete;
    HashAssigner& operator=(const HashAssigner &) = delete;

    virtual std::string name() const override
    {
        return "Photo hash generation";
    }

    virtual void perform() override
    {
        QImage image(m_photoInfo->getPath().c_str());
        const PhotoInfo::Hash hash = HashFunctions::sha256(image.bits(), image.byteCount());
        m_photoInfo->setHash(hash);
    }

    PhotoInfo::Ptr m_photoInfo;
};


PhotoInfoUpdater::PhotoInfoUpdater()
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateHash(const PhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<HashAssigner>(photoInfo);
    TaskExecutorConstructor::get()->add(task);
}


void PhotoInfoUpdater::updateThumbnail(const PhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<ThumbnailGenerator>(photoInfo);
    TaskExecutorConstructor::get()->add(task);
}
