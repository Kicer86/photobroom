
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <mutex>
#include <condition_variable>
#include <QTimer>

#include <core/exif_reader_factory.hpp>
#include <core/task_executor_utils.hpp>
#include <core/media_information.hpp>
#include <database/iphoto_info.hpp>
#include <database/idatabase.hpp>

struct ICoreFactoryAccessor;

struct UpdaterTask;


//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final: public QObject
{
    public:
        explicit PhotoInfoUpdater(ICoreFactoryAccessor *, Database::IDatabase* db);
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        void updateSha256(const IPhotoInfo::Ptr &);
        void updateGeometry(const IPhotoInfo::Ptr &);
        void updateTags(const IPhotoInfo::Ptr &);

        int tasksInProgress();
        void dropPendingTasks();
        void waitForActiveTasks();

    private:
        friend struct UpdaterTask;
        typedef std::map<Photo::Id, Photo::DataDelta> TouchedPhotos;

        MediaInformation m_mediaInformation;
        TouchedPhotos m_touchedPhotos;
        QTimer m_cacheFlushTimer;
        TasksQueue m_taskQueue;
        std::set<UpdaterTask *> m_tasks;
        std::mutex m_tasksMutex;
        std::condition_variable m_finishedTask;
        std::thread::id m_threadId;
        ICoreFactoryAccessor* m_coreFactory;
        Database::IDatabase* m_db;

        void taskAdded(UpdaterTask *);
        void taskFinished(UpdaterTask *);
        void apply(const Photo::DataDelta &);
        void flushCache();
        void resetFlushTimer();
};

#endif
