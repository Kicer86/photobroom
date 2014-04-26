
#include "aphoto_info.hpp"

#include <memory>
#include <mutex>

#include <QImage>

#include "tag.hpp"
#include "itagfeeder.hpp"
#include "task_executor.hpp"
#include "hash_functions.hpp"


struct HashAssigner: public ITaskExecutor::ITask
{
    HashAssigner(APhotoInfo *photoInfo): ITask(), m_photoInfo(photoInfo)
    {
    }

    HashAssigner(const HashAssigner &) = delete;
    HashAssigner& operator=(const HashAssigner &) = delete;

    virtual void perform() override
    {
        QImage image(m_photoInfo->getPath().c_str());
        const APhotoInfo::Hash hash = HashFunctions::sha256(image.bits(), image.byteCount());
        m_photoInfo->setHash(hash);
    }

    APhotoInfo* m_photoInfo;
};


struct APhotoInfo::Data
{
    Data(const std::string &p): path(p), tags(), hash(), hashMutex()
    {
        std::unique_ptr<ITagData> p_tags = TagFeederFactory::get()->getTagsFor(p);

        tags = std::move(p_tags);
    }

    Data(const APhotoInfoInitData& initData):
        path(initData.path),
        tags(initData.tags),
        hash(initData.hash),
        hashMutex()
    {
    }

    Data(const Data& other):
        path(other.path),
        tags(other.tags),
        hash(other.hash),
        hashMutex()
    {}

    std::string path;
    std::shared_ptr<ITagData> tags;
    APhotoInfo::Hash hash;

    std::mutex hashMutex;
};


APhotoInfo::APhotoInfo(const std::string &p): QObject(), m_data(new Data(p))
{
    auto task = std::make_shared<HashAssigner>(this);     //calculate hash of 'this'
    TaskExecutorConstructor::get()->add(task);
}


APhotoInfo::APhotoInfo(const APhotoInfoInitData& init): QObject(), m_data(new Data(init))
{
    //TODO: run hash to verify data consistency?
}


APhotoInfo::~APhotoInfo()
{

}


const std::string& APhotoInfo::getPath() const
{
    return m_data->path;
}


std::shared_ptr<ITagData> APhotoInfo::getTags() const
{
    return m_data->tags;
}


const APhotoInfo::Hash& APhotoInfo::getHash() const
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(m_data->hash.empty() == false);
    return m_data->hash;
}


void APhotoInfo::setHash(const Hash& hash)
{
    //hash may be simultaneously read and write, protect it
    std::unique_lock<std::mutex> lock(m_data->hashMutex);

    assert(m_data->hash.empty());
    m_data->hash = hash;

    lock.release(); //write done

    emit updated();
}


APhotoInfoInitData::APhotoInfoInitData(): path(), tags(new TagData), hash()
{

}

