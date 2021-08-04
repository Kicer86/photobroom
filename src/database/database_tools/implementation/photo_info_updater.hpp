
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
    public:
        explicit PhotoInfoUpdater(ITaskExecutor &, ICoreFactoryAccessor *, Database::IDatabase& db);
        ~PhotoInfoUpdater();

        PhotoInfoUpdater(const PhotoInfoUpdater &) = delete;
        PhotoInfoUpdater& operator=(const PhotoInfoUpdater &) = delete;

        void updateSha256(const Photo::SharedData &);
        void updateGeometry(const Photo::SharedData &);
        void updateTags(const Photo::SharedData &);

    private:
        friend struct UpdaterTask;

        MediaInformation m_mediaInformation;
        std::unique_ptr<ILogger> m_logger;
        ICoreFactoryAccessor* m_coreFactory;
        Database::IDatabase& m_db;
        ITaskExecutor& m_tasksExecutor;

        void addTask(std::unique_ptr<UpdaterTask>);
        void applyFlags(const Photo::Id &, const std::pair<QString, int>& generic_flag);
};

#endif
