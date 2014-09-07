
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QPixmap>
#include <QImage>

#include <database/ifs.hpp>
#include <core/task_executor.hpp>
#include <core/hash_functions.hpp>
#include <core/photos_manager.hpp>
#include <core/itagfeeder.hpp>
#include <core/tag.hpp>

namespace
{
    //TODO: remove, use config
    const int photoWidth = 120;
}


struct ThumbnailGenerator: ITaskExecutor::ITask
{
    ThumbnailGenerator(const IPhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo) {}
    virtual ~ThumbnailGenerator() {}

    ThumbnailGenerator(const ThumbnailGenerator &) = delete;
    ThumbnailGenerator& operator=(const ThumbnailGenerator &) = delete;

    virtual std::string name() const override
    {
        return "Photo thumbnail generation";
    }

    virtual void perform() override
    {
        QPixmap pixmap;
        PhotosManager::instance()->getPhoto(m_photoInfo, &pixmap);

        const QPixmap thumbnail = pixmap.scaled(photoWidth, photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->initThumbnail(thumbnail);
    }

    IPhotoInfo::Ptr m_photoInfo;
};


struct HashAssigner: public ITaskExecutor::ITask
{
    HashAssigner(const IPhotoInfo::Ptr& photoInfo): ITask(), m_photoInfo(photoInfo)
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
        QByteArray data;
        PhotosManager::instance()->getPhoto(m_photoInfo, &data);

        const unsigned char* udata = reinterpret_cast<const unsigned char *>(data.constData());
        const IPhotoInfo::Hash hash = HashFunctions::sha256(udata, data.size());
        m_photoInfo->initHash(hash);
    }

    IPhotoInfo::Ptr m_photoInfo;
};


struct TagsCollector: public ITaskExecutor::ITask
{
	TagsCollector(const IPhotoInfo::Ptr& photoInfo) : ITask(), m_photoInfo(photoInfo), m_tagFeederFactory(nullptr)
    {
    }

    TagsCollector(const HashAssigner &) = delete;
    TagsCollector& operator=(const HashAssigner &) = delete;

	void set(ITagFeederFactory* tagFeederFactory)
	{
		m_tagFeederFactory = tagFeederFactory;
	}

    virtual std::string name() const override
    {
        return "Photo tags collection";
    }

    virtual void perform() override
    {
        const QString& path = m_photoInfo->getPath();
        std::shared_ptr<ITagFeeder> feeder = m_tagFeederFactory->get();
        Tag::TagsList p_tags = feeder->getTagsFor(path);

        m_photoInfo->initExifData(p_tags);
    }

    IPhotoInfo::Ptr m_photoInfo;
	ITagFeederFactory* m_tagFeederFactory;
};


PhotoInfoUpdater::PhotoInfoUpdater(): m_tagFeederFactory()
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateHash(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<HashAssigner>(photoInfo);
    TaskExecutorConstructor::get()->add(task);
}


void PhotoInfoUpdater::updateThumbnail(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<ThumbnailGenerator>(photoInfo);
    TaskExecutorConstructor::get()->add(task);
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<TagsCollector>(photoInfo);
	task->set(&m_tagFeederFactory);

    TaskExecutorConstructor::get()->add(task);
}


