
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


TagData::TagsList TagData::getTags() const
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


ITagData::TagsList TagDataComposite::getTags() const
{
    ITagData::TagsList result;
    
    
    for(ITagData *tag: m_tags)
    {
        
    }
    
    return result;
}


void TagDataComposite::setTag(const QString &name, const QString &values)
{
    for(ITagData *tag: m_tags)
        tag->setTag(name, values);
}


