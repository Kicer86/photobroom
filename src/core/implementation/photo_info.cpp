
#include "photo_info.hpp"

#include <memory>

#include "tag.hpp"
#include "tag_feeder.hpp"


struct APhotoInfo::Data
{
    Data(const std::string &p): path(p), tags()
    {
        std::unique_ptr<ITagData> p_tags = TagFeeder::getTagsFor(p);

        tags = std::move(p_tags);
    }

    Data(const Data& other):
        path(other.path),
        tags(other.tags)
    {}

    std::string path;
    std::shared_ptr<ITagData> tags;
};


//TODO: scaling in thread, temporary bitmap

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
