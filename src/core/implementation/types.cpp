
#include "types.hpp"

#include "algo.hpp"

ITagData::~ITagData()
{

}


TagData::TagData(): ITagData(), m_tags()
{
}


TagData::~TagData()
{
    
}


const TagData::TagsList& TagData::getTags() const
{   
    return m_tags;
}
    

void TagData::setTag(const QString &name, const QString &values)
{
    m_tags[name] = values;
}


/*****************************************************************************/


TagDataComposite::TagDataComposite()
{
}


TagDataComposite::~TagDataComposite()
{
}


void TagDataComposite::setTagDatas(const std::vector<ITagData *> &tags)
{
    m_tags = tags;
}


const ITagData::TagsList& TagDataComposite::getTags() const
{
    std::vector<TagData::TagInfo> result;
    
    
    for(ITagData *tag: m_tags)
    {
        
    }
}


void TagDataComposite::setTag(const QString &name, const QString &values)
{
    for(ITagData *tag: m_tags)
        tag->setTag(name, values);
}


