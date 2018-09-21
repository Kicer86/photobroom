
#include "tag.hpp"

#include <QDate>
#include <QString>
#include <QStringList>
#include <QTime>

#include "base_tags.hpp"


TagNameInfo::TagNameInfo(): m_tag(BaseTagsList::Invalid)
{

}


TagNameInfo::TagNameInfo(const BaseTagsList& tag): m_tag(tag)
{

}


TagNameInfo::TagNameInfo(const TagNameInfo& other): m_tag(other.m_tag)
{

}

/*
TagNameInfo::operator QString() const
{
    return getName();
}
*/

bool TagNameInfo::operator==(const TagNameInfo& other) const
{
    const bool result = getName() == other.getName();

    return result;
}


bool TagNameInfo::operator<(const TagNameInfo& other) const
{
    const bool result = getName() < other.getName();

    return result;
}


bool TagNameInfo::operator>(const TagNameInfo& other) const
{
    const bool result = getName() > other.getName();

    return result;
}


TagNameInfo& TagNameInfo::operator=(const TagNameInfo& other)
{
    m_tag = other.m_tag;

    return *this;
}


QString TagNameInfo::getName() const
{
    return BaseTags::getName(m_tag);
}


QString TagNameInfo::getDisplayName() const
{
    return BaseTags::getTr(m_tag);
}


TagNameInfo::Type TagNameInfo::getType() const
{
    return BaseTags::getType(m_tag);
}


BaseTagsList TagNameInfo::getTag() const
{
    return m_tag;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_type(Type::Empty), m_value()
{

}


TagValue::TagValue(const TagValue& other): TagValue()
{
    m_type = other.m_type;
    m_value = other.m_value;
}


TagValue::TagValue(TagValue&& other): TagValue()
{
    m_type = other.m_type;
    other.m_type = Type::Empty;

    std::swap(m_value, other.m_value);
}


TagValue::TagValue(const QDate& date): TagValue()
{
    set(date);
}


TagValue::TagValue(const QTime& time): TagValue()
{
    set(time);
}


TagValue::TagValue(const QString& string): TagValue()
{
    set(string);
}


TagValue TagValue::fromRaw(const QString& raw, const TagNameInfo::Type& type)
{
    return TagValue().fromString(raw, type);
}


TagValue TagValue::fromQVariant(const QVariant& variant)
{
    TagValue result;

    const QVariant::Type type = variant.type();

    switch(type)
    {
        default:
            assert(!"unknown type");
            break;

        case QVariant::String:
            result = TagValue( variant.toString() );
            break;

        case QVariant::Date:
            result = TagValue( variant.toDate() );
            break;

        case QVariant::Time:
            result = TagValue( variant.toTime() );
            break;
    }

    return result;
}


TagValue::~TagValue()
{

}


TagValue& TagValue::operator=(const TagValue& other)
{
    m_type = other.m_type;
    m_value = other.m_value;

    return *this;
}


TagValue& TagValue::operator=(TagValue&& other)
{
    m_type = other.m_type;
    other.m_type = Type::Empty;

    std::swap(m_value, other.m_value);

    return *this;
}


void TagValue::set(const QDate& date)
{
    m_value = date;
    m_type = Type::Date;
}


void TagValue::set(const QTime& time)
{
    m_value = time;
    m_type = Type::Time;
}


void TagValue::set(const QString& string)
{
    m_value = string;
    m_type = Type::String;
}


QVariant TagValue::get() const
{
    QVariant result;

    switch (m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
            result = * get<TagValueTraits<Type::Date>::StorageType>();
            break;

        case Type::String:
            result = * get<TagValueTraits<Type::String>::StorageType>();
            break;

        case Type::Time:
            result = * get<TagValueTraits<Type::Time>::StorageType>();
            break;
    }

    return result;
}


const QDate& TagValue::getDate() const
{
    auto* v = get<TagValueTraits<Type::Date>::StorageType>();

    return *v;
}


const QString& TagValue::getString() const
{
    auto* v = get<TagValueTraits<Type::String>::StorageType>();

    return *v;
}


const QTime& TagValue::getTime() const
{
    auto* v = get<TagValueTraits<Type::Time>::StorageType>();

    return *v;
}


QDate& TagValue::getDate()
{
    auto* v = get<TagValueTraits<Type::Date>::StorageType>();

    return *v;
}


QString& TagValue::getString()
{
    auto* v = get<TagValueTraits<Type::String>::StorageType>();

    return *v;
}


QTime& TagValue::getTime()
{
    auto* v = get<TagValueTraits<Type::Time>::StorageType>();

    return *v;
}


TagValue::Type TagValue::type() const
{
    return m_type;
}


QString TagValue::rawValue() const
{
    return string();
}


bool TagValue::operator==(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString == otherString;
}


bool TagValue::operator!=(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString != otherString;
}


bool TagValue::operator<(const TagValue& other) const
{
    const QString thisString = string();
    const QString otherString = other.string();

    return thisString < otherString;
}


template<>
bool TagValue::validate<QDate>() const
{
    return m_type == Type::Date && m_value.has_value() && m_value.type() == typeid(QDate);
}


template<>
bool TagValue::validate<QTime>() const
{
    return m_type == Type::Time && m_value.has_value() && m_value.type() == typeid(QTime);
}


template<>
bool TagValue::validate<QString>() const
{
    return m_type == Type::String && m_value.has_value() && m_value.type() == typeid(QString);
}


QString TagValue::string() const
{
    QString result;

    switch(m_type)
    {
        case Type::Empty:
            break;

        case Type::Date:
        {
            const QDate* v = get<TagValueTraits<Type::Date>::StorageType>();
            result = v->toString("yyyy.MM.dd");
            break;
        }

        case Type::String:
        {
            const QString* v = get<TagValueTraits<Type::String>::StorageType>();
            result = *v;
            break;
        }

        case Type::Time:
        {
            const QTime* v = get<TagValueTraits<TagValue::Type::Time>::StorageType>();
            result = v->toString("HH:mm:ss");
            break;
        }
    }

    return result;
}


TagValue& TagValue::fromString(const QString& value, const TagNameInfo::Type& type)
{
    switch(type)
    {
        case TagNameInfo::Type::String:
            set( value );
            break;

        case TagNameInfo::Type::Date:
            set( QDate::fromString(value, "yyyy.MM.dd") );
            break;

        case TagNameInfo::Type::Time:
            set( QTime::fromString(value, "HH:mm:ss") );
            break;

        case TagNameInfo::Type::Invalid:
            assert(!"Unexpected switch");
            break;
    }

    return *this;
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

    Info::Info(const TagNameInfo& n, const TagValue& v): m_name(n), m_value(v)
    {

    }

    Info& Info::operator=(const std::pair<TagNameInfo, TagValue> &data)
    {
        m_name = data.first;
        m_value = data.second;

        return *this;
    }

    QString Info::name() const
    {
        return m_name.getName();
    }

    QString Info::displayName() const
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

    void Info::setValue(const TagValue& v)
    {
        m_value = v;
    }
}
