
#include "ibackend.hpp"
#include "idatabase_builder.hpp"
#include "iphoto_info.hpp"
#include "iphoto_info_cache.hpp"

namespace Database
{
    IBackend::~IBackend() {}
    IBuilder::~IBuilder() {}

    AStorePhotoTask::~AStorePhotoTask()       {}
    AStoreTagTask::~AStoreTagTask()           {}
    AListTagsTask::~AListTagsTask()           {}
    AListTagValuesTask::~AListTagValuesTask() {}
    AGetPhotosTask::~AGetPhotosTask()         {}
    AGetPhotoTask::~AGetPhotoTask()           {}
    AGetPhotosCount::~AGetPhotosCount()       {}
    ADropPhotosTask::~ADropPhotosTask()       {}
    AInitTask::~AInitTask()                   {}
    IDatabase::~IDatabase()                   {}
    IPhotoInfoCache::~IPhotoInfoCache()       {}
}

IPhotoInfo::IObserver::~IObserver() {}
IPhotoInfo::~IPhotoInfo()           {}
