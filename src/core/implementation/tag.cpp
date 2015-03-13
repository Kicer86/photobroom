
#include "tag.hpp"

#include <QString>
#include <QStringList>


TagNameInfo::TagNameInfo(): name(), type(Invalid)
{

}


TagNameInfo::TagNameInfo(const QString& n, const Type t): name(n), type(t)
{

}


TagNameInfo::TagNameInfo(const QString& n, int t): TagNameInfo(n, Type(t))
{

}


TagNameInfo::TagNameInfo(const TagNameInfo& other): name(other.name), type(other.type)
{

}


TagNameInfo::operator QString() const
{
    return name;
}


bool TagNameInfo::operator==(const TagNameInfo& other) const
{
    const bool result = name == other.name;

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
    type = other.type;

    return *this;
}


const QString& TagNameInfo::getName() const
{
    return name;
}


TagNameInfo::Type TagNameInfo::getType() const
{
    return type;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_values()
{

}


TagValue::TagValue(const QString& value): m_values( {value} )
{
}


TagValue::~TagValue()
{

}


void TagValue::set(const QString& value)
{
    m_values.clear();
    m_values.insert(value);
}


const QString TagValue::get() const
{
    const QString r = m_values.empty()? "": *m_values.begin();

    return r;
}


const TagValue::List& TagValue::getAll() const
{
    return m_values;
}


bool TagValue::operator==(const TagValue& other) const
{
    return m_values == other.m_values;
}


bool TagValue::operator!=(const TagValue& other) const
{
    return m_values != other.m_values;
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const Tag::TagsList::const_iterator &it): m_name(it->first), m_value(it->second) {}
    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_value(data.second) {}

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
    {
        m_name = data.first;
        m_value = data.second;

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

    const TagValue& Info::value() const
    {
        return m_value;
    }


    void Info::setValue(const QString& v)
    {
        m_value.set(v);
    }

}
