
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <mutex>
#include <condition_variable>

#include <core/exif_reader_factory.hpp>
#include <core/task_executor.hpp>
#include <core/media_information.hpp>
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
        void updateGeometry(const IPhotoInfo::Ptr &);
        void updateTags(const IPhotoInfo::Ptr &);

        void set(ITaskExecutor *);
        void set(IConfiguration *);

        int tasksInProgress();
        void dropPendingTasks();
        void waitForActiveTasks();

    private:
        friend struct UpdaterTask;

        MediaInformation m_mediaInformation;
        ExifReaderFactory m_exifReaderFactory;
        ITaskExecutor::TaskQueue m_taskQueue;
        std::set<UpdaterTask *> m_tasks;
        std::mutex m_tasksMutex;
        std::condition_variable m_finishedTask;
        IConfiguration* m_configuration;

        void taskAdded(UpdaterTask *);
        void taskFinished(UpdaterTask *);
};

#endif
