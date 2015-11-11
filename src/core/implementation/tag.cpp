
#include "tag.hpp"

#include <QString>
#include <QStringList>

#include "base_tags.hpp"


MultipleValues::MultipleValues(): m_userType(QVariant::Invalid)
{
    
}


MultipleValues::MultipleValues(int userType): m_userType(userType)
{

}


MultipleValues::~MultipleValues()
{

}


int MultipleValues::userType() const
{
    return m_userType;
}


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
        validate();
    }

    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_value(data.second)
    {
        validate();
    }

    Info::Info(const TagNameInfo& n, const QVariant& v): m_name(n), m_value()
    {
        setValue(v);
        validate();
    }

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
    {
        m_name = data.first;
        m_value = data.second;

        validate();

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
        validate();
    }

    void Info::validate() const
    {
#ifndef NDEBUG
        bool ok = false;
        const auto expectedValueType = m_name.getType();
        const auto currentValueType = m_value.get().userType();

        if (currentValueType == qMetaTypeId<MultipleValues>())
            ok = true;
        else
            switch (expectedValueType)
            {
                case TagNameInfo::Invalid:
                    ok = currentValueType == QVariant::Invalid;
                    break;

                case TagNameInfo::Text:
                    ok = currentValueType == QVariant::String;
                    break;

                case TagNameInfo::Date:
                    ok = currentValueType == QVariant::Date;
                    break;

                case TagNameInfo::Time:
                    ok = currentValueType == QVariant::Time;
                    break;

                case TagNameInfo::List:
                    ok = currentValueType == QVariant::StringList;
                    break;
            }

        if (ok == false)
            std::abort();
#endif
    }


}
