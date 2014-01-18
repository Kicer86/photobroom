
#include "photo_info.hpp"

#include <memory>
#include <mutex>

#include "tag.hpp"
#include "tag_feeder.hpp"


struct APhotoInfo::Data
{
    Data(const std::string &p): path(p), tags(), hash(), hashMutex()
    {
        std::unique_ptr<ITagData> p_tags = TagFeeder::getTagsFor(p);

        tags = std::move(p_tags);
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


APhotoInfo::APhotoInfo(const std::string &p): m_data(new Data(p))
{

}


APhotoInfo::APhotoInfo(const APhotoInfo& other): m_data( new Data(*other.m_data.get()) )
{

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
}
