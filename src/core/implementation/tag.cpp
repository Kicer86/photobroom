
#include "tag.hpp"


TagNameInfo::TagNameInfo(const QString& n, const Type t, char s): name(n), type(t), separator(s)
{

}


TagNameInfo::TagNameInfo(const QString& n, int t, char s): TagNameInfo(n, Type(t), s)
{

}


TagNameInfo::TagNameInfo(const TagNameInfo& other): name(other.name), type(other.type), separator(other.separator)
{

}


TagNameInfo::operator QString() const
{
    return name;
}


bool TagNameInfo::operator==(const TagNameInfo& other) const
{
    const bool result = name == other.name;

    assert(result == false || separator == other.separator);  //if result is true, then separators must be equal

    return result;
}


bool TagNameInfo::operator<(const TagNameInfo& other) const
{
    const bool result = name < other.name;

    return result;
}


TagNameInfo& TagNameInfo::operator=(const TagNameInfo& other)
{
    name = other.name;
    separator = other.separator;
    type = other.type;

    return *this;
}


const QString& TagNameInfo::getName() const
{
    return name;
}


char TagNameInfo::getSeparator() const
{
    return separator;
}


TagNameInfo::Type TagNameInfo::getType() const
{
    return type;
}


//////////////////////////////////////////////////////////////


TagValueInfo::TagValueInfo(const QString& v): m_value(v) {}

TagValueInfo::operator QString() const
{
    return m_value;
}

bool TagValueInfo::operator<(const TagValueInfo& other) const
{
    const bool result = m_value < other.m_value;

    return result;
}

const QString& TagValueInfo::value() const
{
    return m_value;
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const Tag::TagsList::const_iterator &it): m_name(it->first), m_values(it->second) {}
    Info::Info(const std::pair<TagNameInfo, Tag::ValuesSet> &data): m_name(data.first), m_values(data.second) {}

    Info& Info::operator=(const std::pair<TagNameInfo, Tag::ValuesSet> &data)
    {
        m_name = data.first;
        m_values = data.second;

        return *this;
    }

    QString Info::name() const
    {
        return m_name;
    }

    TagNameInfo Info::getTypeInfo() const
    {
        return m_name;
    }

    const Tag::ValuesSet& Info::values() const
    {
        return m_values;
    }

    QString Info::valuesString() const
    {
        QString result;

        for(const QString &str: m_values)
            result += str + m_name.getSeparator() + " ";

        return result.simplified();
    }

}
