
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <core/tag_feeder_factory.hpp>
#include <database/iphoto_info.hpp>

struct IConfiguration;
struct ITaskExecutor;

struct BaseTask;

struct ITaskObserver
{
    virtual ~ITaskObserver() {}

    virtual void finished(BaseTask *) = 0;
};


//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final: ITaskObserver
{
    public:
        PhotoInfoUpdater();
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        void updateHash(const IPhotoInfo::Ptr &);
        void updateThumbnail(const IPhotoInfo::Ptr &);
        void updateTags(const IPhotoInfo::Ptr &);

        void set(ITaskExecutor *);
        void set(IConfiguration *);

        int tasksInProgress();

    private:
        TagFeederFactory m_tagFeederFactory;
        ITaskExecutor* m_task_executor;
        IConfiguration* m_configuration;
        ol::ThreadSafeResource<std::set<BaseTask *>> m_runningTasks;

        void started(BaseTask *);
        void finished(BaseTask *) override;
};

#endif
