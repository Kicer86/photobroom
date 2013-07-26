
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
    std::vector<TagInfo> results;
    
    for(auto &entry: m_tags)
    {
        TagInfo info;
        
        info.name = entry.first;
        info.values = entry.second;
        
        results.push_back(info);
    }
    
    return results;
}
    

void TagData::setTag(const QString &name, const QString &values)
{
    m_tags[name] = values;
}
