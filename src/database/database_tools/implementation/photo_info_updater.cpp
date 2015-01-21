
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


struct BaseTask: ITaskExecutor::ITask
{
    BaseTask(ITaskObserver* observer): m_observer(observer) {}
    BaseTask(const BaseTask &) = delete;

    virtual ~BaseTask()
    {
        m_observer->finished(this);
    }

    BaseTask& operator=(const BaseTask &) = delete;

    ITaskObserver* m_observer;
};


struct ThumbnailGenerator: BaseTask
{
    ThumbnailGenerator(ITaskObserver* observer, const IPhotoInfo::Ptr& photoInfo, int photoWidth): BaseTask(observer), m_photoInfo(photoInfo), m_photoWidth(photoWidth) {}
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


struct HashAssigner: public BaseTask
{
    HashAssigner(ITaskObserver* observer, const IPhotoInfo::Ptr& photoInfo): BaseTask(observer), m_photoInfo(photoInfo)
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


struct TagsCollector: public BaseTask
{
    TagsCollector(ITaskObserver* observer, const IPhotoInfo::Ptr& photoInfo) : BaseTask(observer), m_photoInfo(photoInfo), m_tagFeederFactory(nullptr)
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
        m_photoInfo->markFlag(IPhotoInfo::FlagsE::ExifLoaded, 1);
    }

    IPhotoInfo::Ptr m_photoInfo;
    ITagFeederFactory* m_tagFeederFactory;
};


PhotoInfoUpdater::PhotoInfoUpdater(): m_tagFeederFactory(), m_task_executor(nullptr), m_configuration(nullptr), m_runningTasks(), m_pendingTasksMutex(), m_pendigTasksNotifier()
{

}


PhotoInfoUpdater::~PhotoInfoUpdater()
{

}


void PhotoInfoUpdater::updateHash(const IPhotoInfo::Ptr& photoInfo)
{
    std::unique_ptr<HashAssigner> task(new HashAssigner(this, photoInfo));

    started(task.get());
    m_task_executor->add(std::move(task));
}


void PhotoInfoUpdater::updateThumbnail(const IPhotoInfo::Ptr& photoInfo)
{
    auto widthEntry = m_configuration->findEntry(Configuration::BasicKeys::thumbnailWidth);
    int width = 120;

    if (widthEntry)
        width = widthEntry->toInt();

    std::unique_ptr<ThumbnailGenerator> task(new ThumbnailGenerator(this, photoInfo, width));

    started(task.get());
    m_task_executor->add(std::move(task));
}


void PhotoInfoUpdater::updateTags(const IPhotoInfo::Ptr& photoInfo)
{
    std::unique_ptr<TagsCollector> task(new TagsCollector(this, photoInfo));
    task->set(&m_tagFeederFactory);

    started(task.get());
    m_task_executor->add(std::move(task));
}


void PhotoInfoUpdater::set(ITaskExecutor* taskExecutor)
{
    m_task_executor = taskExecutor;
}


void PhotoInfoUpdater::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


int PhotoInfoUpdater::tasksInProgress()
{
    return m_runningTasks.lock()->size();
}


void PhotoInfoUpdater::waitForPendingTasks()
{
    std::unique_lock<std::mutex> tasks_lock(m_pendingTasksMutex);
    m_pendigTasksNotifier.wait(tasks_lock, [&]
    {
        const int tasks = tasksInProgress();
        std::clog << "PhotoInfoUpdater: " << tasks << " left" << std::endl;

        return tasks == 0;
    });
}


void PhotoInfoUpdater::started(BaseTask* task)
{
    m_runningTasks.lock()->insert(task);
}


void PhotoInfoUpdater::finished(BaseTask* task)
{
    auto tasks = m_runningTasks.lock();
    auto it = tasks->find(task);

    assert(it != tasks->cend());

    tasks->erase(it);

    m_pendigTasksNotifier.notify_one();
}
