
#include "tag.hpp"

#include <QDate>
#include <QString>
#include <QStringList>
#include <QTime>

#include "base_tags.hpp"
#include <variant_converter.hpp>


TagNameInfo::TagNameInfo(): name(), displayName(), type(TagType::Empty)
{

}


TagNameInfo::TagNameInfo(const QString& n, const TagType t, const QString& d): name(n), displayName(d), type(t)
{

}



TagNameInfo::TagNameInfo(const QString& n, const TagType t): name(n), displayName(dn(n)), type(t)
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


bool TagNameInfo::operator>(const TagNameInfo& other) const
{
    const bool result = name > other.name;

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



TagType TagNameInfo::getType() const
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


TagValue::TagValue(): m_type(TagType::Empty), m_value(nullptr)
{

}


TagValue::TagValue(const TagValue& other): TagValue()
{
    copy(other);
}


TagValue::TagValue(TagValue&& other)
{
    m_type = other.m_type;
    m_value = other.m_value;

    other.m_type = TagType::Empty;
    other.m_value = nullptr;
}


TagValue::TagValue(const QVariant& value): TagValue()
{
    set(value);
}


TagValue::~TagValue()
{
    destroyValue();
}


TagValue& TagValue::operator=(const TagValue& other)
{
    copy(other);

    return *this;
}


TagValue& TagValue::operator=(TagValue&& other)
{
    m_type = other.m_type;
    m_value = other.m_value;

    other.m_type = TagType::Empty;
    other.m_value = nullptr;
}


void TagValue::set(const QVariant& v)
{
    const QVariant::Type type = v.type();

    switch(type)
    {
        default:
            assert(!"unknown type");

        case QVariant::String:
            set(v.toString());
            break;

        case QVariant::Date:
            set(v.toDate());
            break;

        case QVariant::Time:
            set(v.toTime());
            break;

        case QVariant::StringList:
        {
            const QStringList l = v.toStringList();

            std::deque<TagValue> values;

            for(const QString& str: l)
            {
                const TagValue tagValue(str);

                values.push_back(tagValue);
            }

            set(values);
            break;
        }
    }
}


void TagValue::set(const QDate& date)
{
    destroyValue();

    QDate* v = new QDate(date);
    m_value = v;
    m_type = TagType::Date;
}


void TagValue::set(const QTime& time)
{
    destroyValue();

    QTime* v = new QTime(time);
    m_value = v;
    m_type = TagType::Time;
}


void TagValue::set(const std::deque<TagValue>& values)
{
    destroyValue();

    std::deque<TagValue>* v = new std::deque<TagValue>(values);
    m_value = v;
    m_type = TagType::List;
}


void TagValue::set(const QString& string)
{
    destroyValue();

    QString* v = new QString(string);
    m_value = v;
    m_type = TagType::String;
}


QVariant TagValue::get() const
{
    QVariant result;

    switch (m_type)
    {
        case TagType::Empty:
            break;

        case TagType::Date:
            result = * get<TagValueTraits<TagType::Date>::StorageType>();
            break;

        case TagType::List:
        {
            QStringList localResult;
            std::deque<TagValue>* v = get<TagValueTraits<TagType::List>::StorageType>();

            for(const TagValue& tagValue: *v)
                localResult.append(tagValue.string());

            result = localResult;
            break;
        }

        case TagType::String:
            result = * get<TagValueTraits<TagType::String>::StorageType>();
            break;

        case TagType::Time:
            result = * get<TagValueTraits<TagType::Time>::StorageType>();
            break;
    }

    return result;
}


QDate TagValue::getDate() const
{
    auto* v = get<TagValueTraits<TagType::Date>::StorageType>();

    return *v;
}


std::deque<TagValue> TagValue::getList() const
{
    auto* v = get<TagValueTraits<TagType::List>::StorageType>();

    return *v;
}


QString TagValue::getString() const
{
    auto* v = get<TagValueTraits<TagType::String>::StorageType>();

    return *v;
}


QTime TagValue::getTime() const
{
    auto* v = get<TagValueTraits<TagType::Time>::StorageType>();

    return *v;
}


TagType TagValue::type() const
{
    return m_type;
}


QString TagValue::formattedValue() const
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


void TagValue::destroyValue()
{
    switch (m_type)
    {
        case TagType::Empty:
            break;

        case TagType::Date:
            delete get<TagValueTraits<TagType::Date>::StorageType>();
            break;

        case TagType::List:
        {
            auto* v = get<TagValueTraits<TagType::List>::StorageType>();

            for(TagValue& tagValue: *v)
                tagValue.destroyValue();

            delete v;
            break;
        }

        case TagType::String:
            delete get<TagValueTraits<TagType::String>::StorageType>();
            break;

        case TagType::Time:
            delete get<TagValueTraits<TagType::Time>::StorageType>();
            break;
    }

    m_type = TagType::Empty;
    m_value = nullptr;
}


void TagValue::copy(const TagValue& other)
{
    switch (other.m_type)
    {
        case TagType::Empty:
            break;

        case TagType::Date:
            set(* other.get<TagValueTraits<TagType::Date>::StorageType>() );
            break;

        case TagType::List:
            set(* other.get<TagValueTraits<TagType::List>::StorageType>() );
            break;

        case TagType::String:
            set(* other.get<TagValueTraits<TagType::String>::StorageType>() );
            break;

        case TagType::Time:
            set(* other.get<TagValueTraits<TagType::Time>::StorageType>() );
            break;
    }

    m_type = other.m_type;
}


template<>
bool TagValue::validate<QDate>() const
{
    return m_type == TagType::Date && m_value != nullptr;
}


template<>
bool TagValue::validate<QTime>() const
{
    return m_type == TagType::Time && m_value != nullptr;
}


template<>
bool TagValue::validate<QString>() const
{
    return m_type == TagType::String && m_value != nullptr;
}


template<>
bool TagValue::validate<std::deque<TagValue>>() const
{
    return m_type == TagType::List && m_value != nullptr;
}


QString TagValue::string() const
{
    QString result;

    switch(m_type)
    {
        case TagType::Empty:
            break;

        case TagType::Date:
        {
            QDate* v = get<TagValueTraits<TagType::Date>::StorageType>();
            result = v->toString("yyyy.MM.dd");;
            break;
        }

        case TagType::List:
        {
            QString localResult;
            auto* v = get<TagValueTraits<TagType::List>::StorageType>();

            for(const TagValue& tagValue: *v)
                localResult += tagValue.string() + '\0';

            result = localResult;
            break;
        }

        case TagType::String:
        {
            QString* v = get<TagValueTraits<TagType::String>::StorageType>();
            result = *v;
            break;
        }

        case TagType::Time:
        {
            QTime* v = get<TagValueTraits<TagType::Time>::StorageType>();
            result = v->toString("HH:mm:ss");
            break;
        }
    }

    return result;
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
