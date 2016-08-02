
#include "tag.hpp"

#include <QDate>
#include <QString>
#include <QStringList>
#include <QTime>

#include "base_tags.hpp"


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


TagValue::TagValue(): m_type(TagType::Empty), m_value()
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
    other.m_type = TagType::Empty;

    boost::swap(m_value, other.m_value);
}


TagValue::TagValue(const QDate& date): TagValue()
{
    set(date);
}


TagValue::TagValue(const QTime& time): TagValue()
{
    set(time);
}


TagValue::TagValue(const std::deque<TagValue>& list): TagValue()
{
    set(list);
}


TagValue::TagValue(const QString& string): TagValue()
{
    set(string);
}


TagValue TagValue::fromRaw(const QString& raw, const TagType& type)
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

        case QVariant::StringList:
        {
            std::deque<TagValue> list;
            QStringList stringList = variant.toStringList();

            list.insert(list.end(), stringList.begin(), stringList.end());

            result = TagValue(list);
            break;
        }

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
    other.m_type = TagType::Empty;

    boost::swap(m_value, other.m_value);

    return *this;
}


void TagValue::set(const QDate& date)
{
    m_value = date;
    m_type = TagType::Date;
}


void TagValue::set(const QTime& time)
{
    m_value = time;
    m_type = TagType::Time;
}


void TagValue::set(const std::deque<TagValue>& values)
{
    m_value = values;
    m_type = TagType::List;
}


void TagValue::set(const QString& string)
{
    m_value = string;
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
            const std::deque<TagValue>* v = get<TagValueTraits<TagType::List>::StorageType>();

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


const QDate& TagValue::getDate() const
{
    auto* v = get<TagValueTraits<TagType::Date>::StorageType>();

    return *v;
}


const std::deque<TagValue>& TagValue::getList() const
{
    auto* v = get<TagValueTraits<TagType::List>::StorageType>();

    return *v;
}


const QString& TagValue::getString() const
{
    auto* v = get<TagValueTraits<TagType::String>::StorageType>();

    return *v;
}


const QTime& TagValue::getTime() const
{
    auto* v = get<TagValueTraits<TagType::Time>::StorageType>();

    return *v;
}


QDate& TagValue::getDate()
{
    auto* v = get<TagValueTraits<TagType::Date>::StorageType>();

    return *v;
}


std::deque< TagValue >& TagValue::getList()
{
    auto* v = get<TagValueTraits<TagType::List>::StorageType>();

    return *v;
}


QString& TagValue::getString()
{
    auto* v = get<TagValueTraits<TagType::String>::StorageType>();

    return *v;
}


QTime& TagValue::getTime()
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


template<>
bool TagValue::validate<QDate>() const
{
    return m_type == TagType::Date && m_value.empty() == false && m_value.type() == typeid(QDate);
}


template<>
bool TagValue::validate<QTime>() const
{
    return m_type == TagType::Time && m_value.empty() == false && m_value.type() == typeid(QTime);
}


template<>
bool TagValue::validate<QString>() const
{
    return m_type == TagType::String && m_value.empty() == false && m_value.type() == typeid(QString);
}


template<>
bool TagValue::validate<std::deque<TagValue>>() const
{
    return m_type == TagType::List && m_value.empty() == false && m_value.type() == typeid(std::deque<TagValue>);
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
            const QDate* v = get<TagValueTraits<TagType::Date>::StorageType>();
            result = v->toString("yyyy.MM.dd");;
            break;
        }

        case TagType::List:
        {
            QString localResult;
            auto* v = get<TagValueTraits<TagType::List>::StorageType>();

            for(const TagValue& tagValue: *v)
                localResult += tagValue.string() + '\n';

            result = localResult;
            break;
        }

        case TagType::String:
        {
            const QString* v = get<TagValueTraits<TagType::String>::StorageType>();
            result = *v;
            break;
        }

        case TagType::Time:
        {
            const QTime* v = get<TagValueTraits<TagType::Time>::StorageType>();
            result = v->toString("HH:mm:ss");
            break;
        }
    }

    return result;
}


TagValue& TagValue::fromString(const QString& value, const TagType& type)
{
    m_type = type;

    switch(type)
    {
        case TagType::Empty:
        case TagType::String:
            set( value );
            break;

        case TagType::Date:
            set( QDate::fromString(value, "yyyy.MM.dd") );
            break;

        case TagType::Time:
            set( QTime::fromString(value, "HH:mm:ss") );
            break;

        case TagType::List:
            set( { value} );
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

    void Info::setValue(const TagValue& v)
    {
        m_value = v;
    }

}
