
#include "photo_info_generator.hpp"

#include <assert.h>

#include <QPixmap>
#include <QImage>
#include <QtSvg/QSvgRenderer>

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
    ThumbnailGenerator(const APhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo) {}
    virtual ~ThumbnailGenerator() {}

    ThumbnailGenerator(const ThumbnailGenerator &) = delete;
    ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

    virtual void perform()
    {
        QPixmap pixmap(m_photoInfo->getPath().c_str());
        QPixmap thumbnail = pixmap.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->setThumbnail(thumbnail);
    }

    APhotoInfo::Ptr m_photoInfo;
};

struct HashAssigner: public ITaskExecutor::ITask
{
    HashAssigner(const APhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo)
    {
    }

    HashAssigner(const HashAssigner &) = delete;
    HashAssigner& operator=(const HashAssigner &) = delete;

    virtual void perform() override
    {
        QImage image(m_photoInfo->getPath().c_str());
        const APhotoInfo::Hash hash = HashFunctions::sha256(image.bits(), image.byteCount());
        m_photoInfo->setHash(hash);
    }

    APhotoInfo::Ptr m_photoInfo;
};


PhotoInfoGenerator::PhotoInfoGenerator()
{

}


PhotoInfoGenerator::~PhotoInfoGenerator()
{

}


APhotoInfo::Ptr PhotoInfoGenerator::get(const std::string& path)
{
    APhotoInfo::Ptr result = std::make_shared<APhotoInfo>(path);
    QPixmap tmpThumbnail;
    tmpThumbnail.load(":/gui/images/clock.svg");             //use temporary thumbnail until final one is ready
    result->setThumbnail(tmpThumbnail);

    //add generators
    auto task = std::make_shared<ThumbnailGenerator>(result);  //generate thumbnail
    TaskExecutorConstructor::get()->add(task);

    auto task_hash = std::make_shared<HashAssigner>(result);   //generate hash
    TaskExecutorConstructor::get()->add(task);

    return result;
}

