
#include "tag.hpp"

#include <QColor>
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
    typedef int     IntType;
    typedef QColor  ColorType;
}


TagTypeInfo::TagTypeInfo(): m_tag(Tag::Types::Invalid)
{

}


TagTypeInfo::TagTypeInfo(const Tag::Types& tag): m_tag(tag)
{

}


TagTypeInfo::TagTypeInfo(const TagTypeInfo& other): m_tag(other.m_tag)
{

}

/*
TagNameInfo::operator QString() const
{
    return getName();
}
*/

bool TagTypeInfo::operator==(const TagTypeInfo& other) const
{
    const bool result = getName() == other.getName();

    return result;
}


bool TagTypeInfo::operator<(const TagTypeInfo& other) const
{
    const bool result = getName() < other.getName();

    return result;
}


bool TagTypeInfo::operator>(const TagTypeInfo& other) const
{
    const bool result = getName() > other.getName();

    return result;
}


TagTypeInfo& TagTypeInfo::operator=(const TagTypeInfo& other)
{
    m_tag = other.m_tag;

    return *this;
}


QString TagTypeInfo::getName() const
{
    return BaseTags::getName(m_tag);
}


QString TagTypeInfo::getDisplayName() const
{
    return BaseTags::getTr(m_tag);
}


Tag::Types TagTypeInfo::getTag() const
{
    return m_tag;
}


//////////////////////////////////////////////////////////////


TagValue::TagValue(): m_type(Tag::ValueType::Empty), m_value()
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
    other.m_type = Tag::ValueType::Empty;

    std::swap(m_value, other.m_value);
}


TagValue TagValue::fromRaw(const QString& raw, const Tag::ValueType& type)
{
    return TagValue().fromString(raw, type);
}


TagValue TagValue::fromQVariant(const QVariant& variant)
{
    TagValue result;

    const int type = variant.typeId();

    switch(type)
    {
        default:
            assert(!"unknown type");
            break;

        case QMetaType::Type::QString:
            result = TagValue( variant.toString() );
            break;

        case QMetaType::Type::QDate:
            result = TagValue( variant.toDate() );
            break;

        case QMetaType::Type::QTime:
            result = TagValue( variant.toTime() );
            break;

        case QMetaType::Type::Int:
            result = TagValue( variant.toInt() );
            break;

        case QMetaType::Type::QColor:
        {
            const QColor color = variant.value<QColor>();

            result = TagValue(color);
            break;
        }
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
    other.m_type = Tag::ValueType::Empty;

    std::swap(m_value, other.m_value);

    return *this;
}


QVariant TagValue::get() const
{
    QVariant result;

    switch (m_type)
    {
        case Tag::ValueType::Empty:
            break;

        case Tag::ValueType::Date:
            result = get<DateType>();
            break;

        case Tag::ValueType::String:
            result = get<StringType>();
            break;

        case Tag::ValueType::Time:
            result = get<TimeType>();
            break;

        case Tag::ValueType::Int:
            result = get<IntType>();
            break;

        case Tag::ValueType::Color:
            result = get<ColorType>();
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


Tag::ValueType TagValue::type() const
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
        case Tag::ValueType::Empty:
            break;

        case Tag::ValueType::Date:
        {
            const DateType& v = get<DateType>();
            result = v.toString("yyyy.MM.dd");
            break;
        }

        case Tag::ValueType::String:
        {
            result = get<StringType>();
            break;
        }

        case Tag::ValueType::Time:
        {
            const TimeType& v = get<TimeType>();
            result = v.toString("HH:mm:ss");
            break;
        }

        case Tag::ValueType::Int:
        {
            const IntType v = get<IntType>();
            result = QString::number(v);
            break;
        }

        case Tag::ValueType::Color:
        {
            const QColor color = get<ColorType>();
            const QRgba64 rgba = color.rgba64();
            result = QString::number(rgba);
            break;
        }
    }

    return result;
}


TagValue& TagValue::fromString(const QString& value, const Tag::ValueType& type)
{
    switch(type)
    {
        case Tag::ValueType::String:
            set( value );
            break;

        case Tag::ValueType::Date:
            set( QDate::fromString(value, "yyyy.MM.dd") );
            break;

        case Tag::ValueType::Time:
            set( QTime::fromString(value, "HH:mm:ss") );
            break;

        case Tag::ValueType::Int:
            set( value.toInt() );
            break;

        case Tag::ValueType::Color:
        {
            const quint64 rgba = value.toULongLong();
            const QRgba64 rgba64 = QRgba64::fromRgba64(rgba);
            const QColor color(rgba64);
            set(color);
            break;
        }

        case Tag::ValueType::Empty:
            assert(!"Unexpected switch");
            break;
    }

    return *this;
}
