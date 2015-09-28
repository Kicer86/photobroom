
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <mutex>
#include <condition_variable>

#include <core/tag_feeder_factory.hpp>
#include <core/task_executor.hpp>
#include <database/iphoto_info.hpp>

struct IConfiguration;

struct BaseTask;


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
        TagFeederFactory m_tagFeederFactory;
        ITaskExecutor::TaskQueue m_taskQueue;
        IConfiguration* m_configuration;
};

#endif
