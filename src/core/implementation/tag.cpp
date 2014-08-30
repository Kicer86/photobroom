
#include "tag.hpp"

#include <iostream>
#include <algorithm>

#include "algo.hpp"

ITagData::~ITagData()
{

}


std::ostream& operator<<(std::ostream& stream, const ITagData &tagData)
{
    for (std::pair<TagNameInfo, ITagData::ValuesSet> tags: tagData.getTags())
    {
        stream << tags.first.getName().toStdString() << ": ";

        ITagData::ValuesSet::const_iterator valuesIt = tags.second.cbegin();
        ITagData::ValuesSet::const_iterator valuesEnd = tags.second.cend();

        while (valuesIt != valuesEnd)
        {
            stream << valuesIt->value().toStdString();

            ++valuesIt;

            if (valuesIt != valuesEnd)
                stream << tags.first.getSeparator() << " ";
        }

        stream << std::endl;
    }

    return stream;
}

/*****************************************************************************/


TagDataBase::TagDataBase(): ITagData()
{
}


TagDataBase::TagDataBase(const TagDataBase& other): ITagData()
{
    *this = other;
}


TagDataBase::~TagDataBase()
{
}


void TagDataBase::setTag(const TagNameInfo& name, const TagValueInfo& value)
{
    ValuesSet values;
    values.insert(value);

    this->setTag(name, values);
}


void TagDataBase::setTags(const TagsList& tags)
{
    for(auto tag: tags)
        setTag(tag.first, tag.second);
}


TagDataBase& TagDataBase::operator=(const TagDataBase& other)
{
    clear();

    setTags(other.getTags());
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


void TagData::setTag(const TagNameInfo& name, const ITagData::ValuesSet& values)
{
    m_tags[name] = values;
}


void TagData::clear()
{
    m_tags.clear();
}


bool TagData::isValid() const
{
    return true;
}


/*****************************************************************************/


TagDataComposite::TagDataComposite(): m_tags()
{
}


TagDataComposite::~TagDataComposite()
{
}


void TagDataComposite::setTagDatas(const std::vector<std::shared_ptr<ITagData>>& tags)
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
            result = Algo::map_intersection<TagValueInfo>(result, m_tags[i]->getTags());
    }

    return result;
}


void TagDataComposite::setTag(const TagNameInfo& name, const ValuesSet& values)
{
    for(const std::shared_ptr<ITagData> &tag: m_tags)
        tag->setTag(name, values);
}


void TagDataComposite::clear()
{
    for(const std::shared_ptr<ITagData> &tag: m_tags)
        tag->clear();
}


bool TagDataComposite::isValid() const
{
    bool status = false;

    if (m_tags.empty() == false)
        status = std::all_of(m_tags.begin(), m_tags.end(),
                             [](const std::shared_ptr<ITagData> &tag)
    {
        return tag->isValid();
    } );

    return status;
}
