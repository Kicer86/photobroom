
#include "tag.hpp"

#include <QString>
#include <QStringList>


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


TagValue::TagValue(): m_values()
{

}


TagValue::TagValue(const std::initializer_list<QString>& values): m_values()
{
    m_values = values;
}


TagValue::~TagValue()
{

}


void TagValue::setValue(const QString& value)
{
    m_values.clear();
    addValue(value);
}


void TagValue::setValues(const std::set<QString>& values)
{
    m_values = values;
}


void TagValue::addValue(const QString& value)
{
    m_values.insert(value);
}


const std::set<QString>& TagValue::getValues() const
{
    return m_values;
}


std::set<QString>::const_iterator TagValue::begin() const
{
    return m_values.begin();
}


std::set<QString>::const_iterator TagValue::end() const
{
    return m_values.end();
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const Tag::TagsList::const_iterator &it): m_name(it->first), m_values(it->second) {}
    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_values(data.second) {}

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
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

    const TagValue& Info::values() const
    {
        return m_values;
    }

    QString Info::valuesString() const
    {
        QString result;

        const auto& v = m_values.getValues();
        int i = v.size();
        for(const QString &str: v)
        {
            result += str;
            if (--i > 0)
                result += m_name.getSeparator() + " ";
        }

        return result.simplified();
    }


    bool Info::setRawValues(const QString& rawValues)
    {
        const QStringList splitted = rawValues.split(m_name.getSeparator());

        std::set<QString> valuesSet;
        for(const QString& value: splitted)
            valuesSet.insert(value);

        const bool differs = m_values.getValues() != valuesSet;

        if (differs)
            m_values.setValues(valuesSet);

        return differs;
    }

}
