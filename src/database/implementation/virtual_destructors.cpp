
#include "ibackend.hpp"
#include "idatabase_builder.hpp"
#include "iphoto_info.hpp"

namespace Database
{
    IBackend::~IBackend() {}
    IDBPack::~IDBPack()   {}
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
}

IPhotoInfo::IObserver::~IObserver() {}
IPhotoInfo::~IPhotoInfo()           {}
