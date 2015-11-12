
#include "tag.hpp"

#include <QString>
#include <QStringList>

#include "base_tags.hpp"


TagNameInfo::TagNameInfo(): name(), displayName(), type(Invalid)
{

}


TagNameInfo::TagNameInfo(const QString& n, const TagNameInfo::Type t, const QString& d): name(n), displayName(d), type(t)
{

}



TagNameInfo::TagNameInfo(const QString& n, const Type t): name(n), displayName(dn(n)), type(t)
{

}


TagNameInfo::TagNameInfo(const QString& n, int t): TagNameInfo(n, Type(t))
{

}


TagNameInfo::TagNameInfo(const TagNameInfo& other): name(other.name), displayName(other.displayName), type(other.type)
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
    displayName = other.displayName;

    return *this;
}


const QString& TagNameInfo::getName() const
{
    return name;
}


const QString& TagNameInfo::getDisplayName() const
{
    return displayName;
}



TagNameInfo::Type TagNameInfo::getType() const
{
    return type;
}


QString TagNameInfo::dn(const QString& n) const
{
    // If name is base name, then we have translation.
    // Otherwise we are working with user defined tag

    const TagNameInfo* baseTag = nullptr;
    const auto& all = BaseTags::getAll();

    for(const auto& tag: all)
    {
        if (n == tag)
        {
            baseTag = &tag;
            break;
        }
    }

    //use translation for base. For user's tags use name directly
    const QString result = baseTag == nullptr? n: baseTag->displayName;

    return result;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_value()
{

}


TagValue::TagValue(const QVariant& value): m_value(value)
{

}


TagValue::~TagValue()
{

}


void TagValue::set(const QVariant& value)
{
   m_value = value;
}


const QVariant& TagValue::get() const
{
    return m_value;
}


bool TagValue::operator==(const TagValue& other) const
{
    return m_value == other.m_value;
}


bool TagValue::operator!=(const TagValue& other) const
{
    return m_value != other.m_value;
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const Tag::TagsList::const_iterator &it): m_name(it->first), m_value(it->second)
    {

    }

    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_value(data.second)
    {

    }

    Info::Info(const TagNameInfo& n, const QVariant& v): m_name(n), m_value()
    {
        setValue(v);
    }

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
    {
        m_name = data.first;
        m_value = data.second;

        return *this;
    }

    const QString& Info::name() const
    {
        return m_name.getName();
    }

    const QString& Info::displayName() const
    {
        return m_name.getDisplayName();
    }

    const TagNameInfo& Info::getTypeInfo() const
    {
        return m_name;
    }

    const TagValue& Info::value() const
    {
        return m_value;
    }

    void Info::setValue(const QVariant& v)
    {
        m_value.set(v);
    }

}
