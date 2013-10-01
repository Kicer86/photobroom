
#include "types.hpp"

#include "algo.hpp"

ITagData::~ITagData()
{

}


/*****************************************************************************/


TagDataBase::TagDataBase(): ITagData()
{    
}


TagDataBase::~TagDataBase()
{
}


void TagDataBase::setTag(const ITagData::NameType& name, const ITagData::ValueType& value)
{
    std::set<QString> values;
    values.insert(value);
    
    this->setTag(name, values);
}

/*****************************************************************************/


TagData::TagData(): TagDataBase(), m_tags()
{
}


TagData::~TagData()
{
    
}


TagData::TagsList TagData::getTags() const
{   
    return m_tags;
}
    

void TagData::setTag(const ITagData::NameType& name, const ITagData::ValuesSet& values)
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


void TagDataComposite::setTagDatas(const std::vector<ITagData *>& tags)
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
            result = Algo::map_intersection<QString>(result, m_tags[i]->getTags());
    }
    
    return result;
}


void TagDataComposite::setTag(const NameType& name, const ValuesSet& values)
{
    for(ITagData *tag: m_tags)
        tag->setTag(name, values);
}
