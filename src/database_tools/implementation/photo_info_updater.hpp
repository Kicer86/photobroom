
#ifndef GUI_PHOTO_INFO_HPP
#define GUI_PHOTO_INFO_HPP

#include <core/tag_feeder_factory.hpp>
#include <database/iphoto_info.hpp>

struct IConfiguration;
struct ITaskExecutor;

//TODO: construct photo manualy. Add fillers manualy on demand
class PhotoInfoUpdater final
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

	private:
		TagFeederFactory m_tagFeederFactory;
        ITaskExecutor* m_task_executor;
        IConfiguration* m_configuration;
};

#endif
