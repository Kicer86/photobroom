
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <mutex>
#include <condition_variable>

#include <core/tag_feeder_factory.hpp>
#include <core/task_executor.hpp>
#include <database/iphoto_info.hpp>

struct IConfiguration;

struct UpdaterTask;


//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final
{
    public:
        PhotoInfoUpdater();
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        void updateSha256(const IPhotoInfo::Ptr &);
        void updateThumbnail(const IPhotoInfo::Ptr &);
        void updateTags(const IPhotoInfo::Ptr &);

        void set(ITaskExecutor *);
        void set(IConfiguration *);

        int tasksInProgress();
        void dropPendingTasks();

    private:
        friend struct UpdaterTask;

        TagFeederFactory m_tagFeederFactory;
        ITaskExecutor::TaskQueue m_taskQueue;
        std::set<UpdaterTask *> m_tasks;
        std::mutex m_tasksMutex;
        IConfiguration* m_configuration;

        void taskAdded(UpdaterTask *);
        void taskFinished(UpdaterTask *);
};

#endif
