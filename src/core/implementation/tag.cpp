
#include "tag.hpp"

#include <QDate>
#include <QString>
#include <QStringList>
#include <QTime>

#include "base_tags.hpp"

namespace
{
    typedef QDate   DateType;
    typedef QTime   TimeType;
    typedef QString StringType;
    typedef double  FloatType;
    typedef quint64 Uint64Type;
}


TagNameInfo::TagNameInfo(): m_tag(TagTypes::Invalid)
{

}


TagNameInfo::TagNameInfo(const TagTypes& tag): m_tag(tag)
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


TagTypes TagNameInfo::getTag() const
{
    return m_tag;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_type(Tag::Type::Empty), m_value()
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
    other.m_type = Tag::Type::Empty;

    std::swap(m_value, other.m_value);
}


TagValue TagValue::fromRaw(const QString& raw, const Tag::Type& type)
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

        case QVariant::Double:
            result = TagValue( variant.toDouble() );
            break;

        case QVariant::ULongLong:
            result = TagValue( variant.toULongLong() );
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
    other.m_type = Tag::Type::Empty;

    std::swap(m_value, other.m_value);

    return *this;
}


QVariant TagValue::get() const
{
    QVariant result;

    switch (m_type)
    {
        case Tag::Type::Empty:
            break;

        case Tag::Type::Date:
            result = get<DateType>();
            break;

        case Tag::Type::String:
            result = get<StringType>();
            break;

        case Tag::Type::Time:
            result = get<TimeType>();
            break;

        case Tag::Type::Float:
            result = get<FloatType>();
            break;

        case Tag::Type::Uint64:
            result = get<Uint64Type>();
            break;
    }

    return result;
}


const QDate& TagValue::getDate() const
{
    const auto& v = get<DateType>();

    return v;
}


const QString& TagValue::getString() const
{
    const auto& v = get<StringType>();

    return v;
}


const QTime& TagValue::getTime() const
{
    const auto& v = get<TimeType>();

    return v;
}


Tag::Type TagValue::type() const
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


QString TagValue::string() const
{
    QString result;

    switch(m_type)
    {
        case Tag::Type::Empty:
            break;

        case Tag::Type::Date:
        {
            const DateType& v = get<DateType>();
            result = v.toString("yyyy.MM.dd");
            break;
        }

        case Tag::Type::String:
        {
            result = get<StringType>();
            break;
        }

        case Tag::Type::Time:
        {
            const TimeType& v = get<TimeType>();
            result = v.toString("HH:mm:ss");
            break;
        }

        case Tag::Type::Float:
        {
            const FloatType v = get<FloatType>();
            result = QString::number(v);
            break;
        }

        case Tag::Type::Uint64:
        {
            const Uint64Type v = get<Uint64Type>();
            result = QString::number(v);
            break;
        }
    }

    return result;
}


TagValue& TagValue::fromString(const QString& value, const Tag::Type& type)
{
    switch(type)
    {
        case Tag::Type::String:
            set( value );
            break;

        case Tag::Type::Date:
            set( QDate::fromString(value, "yyyy.MM.dd") );
            break;

        case Tag::Type::Time:
            set( QTime::fromString(value, "HH:mm:ss") );
            break;

        case Tag::Type::Float:
            set( value.toDouble() );
            break;

        case Tag::Type::Uint64:
            set( value.toULongLong() );
            break;

        case Tag::Type::Empty:
            assert(!"Unexpected switch");
            break;
    }

    return *this;
}


//////////////////////////////////////////////////////////////


namespace Tag
{

    Info::Info(const std::pair<const TagNameInfo, TagValue> &data): m_name(data.first), m_value(data.second)
    {

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
