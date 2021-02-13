
#ifndef GUI_PHOTO_INFO_UPDATER_HPP
#define GUI_PHOTO_INFO_UPDATER_HPP

#include <mutex>
#include <condition_variable>
#include <QTimer>

#include <core/exif_reader_factory.hpp>
#include <core/itask_executor.hpp>
#include <core/media_information.hpp>
#include <database/iphoto_info.hpp>
#include <database/idatabase.hpp>

struct ICoreFactoryAccessor;

struct UpdaterTask;


//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final: public QObject
{
        Q_OBJECT

    public:
        explicit PhotoInfoUpdater(ICoreFactoryAccessor *, Database::IDatabase* db);
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        void updateSha256(const Photo::Data &);
        void updateGeometry(const Photo::Data &);
        void updateTags(const Photo::Data &);

        int tasksInProgress();
        void waitForActiveTasks();

    private:
        friend struct UpdaterTask;
        typedef std::map<Photo::Id, Photo::DataDelta> TouchedPhotos;

        MediaInformation m_mediaInformation;
        TouchedPhotos m_touchedPhotos;
        QTimer m_cacheFlushTimer;
        std::set<UpdaterTask *> m_tasks;
        std::mutex m_tasksMutex;
        std::condition_variable m_finishedTask;
        std::thread::id m_threadId;
        std::unique_ptr<ILogger> m_logger;
        ICoreFactoryAccessor* m_coreFactory;
        Database::IDatabase* m_db;
        ITaskExecutor& m_tasksExecutor;

        void addTask(std::unique_ptr<UpdaterTask>);
        void taskFinished(UpdaterTask *);
        void apply(const Photo::DataDelta &);
        void applyFlags(const Photo::Id &, const std::pair<QString, int>& generic_flag);
        void flushCache();
        void resetFlushTimer();

    signals:
        void photoProcessed();
};

#endif
