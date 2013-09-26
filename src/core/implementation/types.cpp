
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


TagDataComposite::TagDataComposite(): m_tags()
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
    
    if (m_tags.size() > 0)
    {
        result = m_tags[0]->getTags();
        
        for(size_t i = 1; i < m_tags.size(); i++)
            Algo::map_intersection(result, m_tags[i]->getTags());
    }
    
    return result;
}


void TagDataComposite::setTag(const QString &name, const QString &values)
{
    for(ITagData *tag: m_tags)
        tag->setTag(name, values);
}


