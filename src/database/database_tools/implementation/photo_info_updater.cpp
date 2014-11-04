
#include "photo_info_updater.hpp"

#include <assert.h>

#include <memory>

#include <QPixmap>
#include <QImage>

#include <core/task_executor.hpp>
#include <core/hash_functions.hpp>
#include <core/photos_manager.hpp>
#include <core/itagfeeder.hpp>
#include <core/tag.hpp>
#include <configuration/iconfiguration.hpp>
#include <configuration/constants.hpp>
#include <database/ifs.hpp>


struct ThumbnailGenerator: ITaskExecutor::ITask
{
    ThumbnailGenerator(const IPhotoInfo::Ptr& photoInfo, int photoWidth): ITask(), m_photoInfo(photoInfo), m_photoWidth(photoWidth) {}
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

        const QPixmap thumbnail = pixmap.scaled(m_photoWidth, m_photoWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        m_photoInfo->initThumbnail(thumbnail);
    }

    IPhotoInfo::Ptr m_photoInfo;
    int m_photoWidth;
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

    TagsCollector(const TagsCollector &) = delete;
    TagsCollector& operator=(const TagsCollector &) = delete;

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

        m_photoInfo->setTags(p_tags);
        m_photoInfo->markExifDataLoaded(true);
    }

    IPhotoInfo::Ptr m_photoInfo;
	ITagFeederFactory* m_tagFeederFactory;
};


PhotoInfoUpdater::PhotoInfoUpdater(): m_tagFeederFactory(), m_task_executor(nullptr), m_configuration(nullptr)
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateHash(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<HashAssigner>(photoInfo);
    m_task_executor->add(task);
}


void PhotoInfoUpdater::updateThumbnail(const IPhotoInfo::Ptr& photoInfo)
{
    auto widthEntry = m_configuration->findEntry(Configuration::BasicKeys::thumbnailWidth);
    int width = 120;

    if (widthEntry)
        width = widthEntry->toInt();

    auto task = std::make_shared<ThumbnailGenerator>(photoInfo, width);
    m_task_executor->add(task);
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    auto task = std::make_shared<TagsCollector>(photoInfo);
	task->set(&m_tagFeederFactory);

    m_task_executor->add(task);
}


void PhotoInfoUpdater::set(ITaskExecutor* taskExecutor)
{
    m_task_executor = taskExecutor;
}


void PhotoInfoUpdater::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}
