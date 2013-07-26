
#include "types.hpp"


ITagData::~ITagData()
{

}


TagData::TagData(): ITagData()
{
}


TagData::~TagData()
{
    
}


std::vector<TagData::TagInfo> TagData::getTags() const
{
}
    

void TagData::setTag(const std::string &name, const std::string &values)
{
}
